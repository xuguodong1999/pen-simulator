#include "az/data/synthesis_tex_generator.h"
#include "az/tex/api.h"
#include "az/core/encoding.h"
#include "az/data/unicode_shape.h"
#include "az/pen/pen_item_info.h"
#include "az/pen/pen_path.h"
#include "az/pen/pen_font.h"
#include "az/js/qjs_wrapper.h"
#include "az/pen/pen_graph.h"

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <stack>

using namespace az;
using namespace az::pen;
using namespace az::data;

using namespace az::tex::mathjax;

//#define XGD_DEBUG

/**
 *      1.2     ->
 * x    3.4     ->
 * ---------    ->
 *      4 8     <-
 *    3 6       <-
 * ---------    ->
 *    4.0 8     <-
 */
static void sort_by_multiply_demonstration(
        std::vector<const NodeProps *> &target,
        az::tex::mathjax::Graph &g
) {
    std::vector<const NodeProps *> mml_id_stack;
    using IdSetType = std::unordered_set<const NodeProps *>;
    std::vector<IdSetType> line_id_sets;
    const NodeProps *line_break_id1 = nullptr;
    const NodeProps *line_break_id2 = nullptr;
    std::unordered_set<const NodeProps *> line_id_set;
    const auto on_discover_node = [&](const NodeProps &node) {
        if (node.svg_type == "line") {
            if (!line_break_id1) {
                line_break_id1 = &node;
            } else {
                line_break_id2 = &node;
            }
        } else if (node.path) {
            line_id_set.insert(&node);
        }
        mml_id_stack.push_back(&node);
    };
    const auto on_finish_node = [&](const NodeProps &node) {
        mml_id_stack.pop_back();
        size_t stack_size = mml_id_stack.size();
        // mml_id_stack=,,,math,mtable,mtr,
        // mml_id_stack=,,,math,mtable,mtr,mtd,...
        // mml_id_stack=,,,math,mtable,mtr,
        if (!line_id_set.empty() && stack_size >= 2
            && mml_id_stack[stack_size - 1]->mml == "mtr"
            && mml_id_stack[stack_size - 2]->mml == "mtable") {
            line_id_sets.emplace_back();
#ifdef XGD_DEBUG
            std::ostringstream out;
            for (auto &item: line_id_set) {
                UCharType uchar_label = convert_unicode_hex_to_str(fmt::format("0x{}", item->label));
                out << uchar_label << ",";
            }
            SPDLOG_INFO("current line set={}", out.str());
#endif
            std::swap(line_id_sets.back(), line_id_set);
        }
    };
    g.traverse(on_discover_node, on_finish_node);
    const auto get_node_priority = [&](const NodeProps *node) -> std::pair<int64_t, int64_t> {
        size_t offset = 0;
        size_t i = 0;
        for (; i < 2; i++) {
            if (i < line_id_sets.size() && line_id_sets[i].contains(node)) {
                return {i + offset, std::stoi(node->id)};
            }
        }
        offset++;
        if (node == line_break_id1) { return {i + offset, 0}; }
        offset++;
        for (; i < line_id_sets.size() - 1; i++) {
            if (i < line_id_sets.size() && line_id_sets[i].contains(node)) {
                return {i + offset, -std::stoi(node->id)};
            }
        }
        offset++;
        if (node == line_break_id2) { return {i + offset, 0}; }
        offset++;
        for (; i < line_id_sets.size(); i++) {
            if (i < line_id_sets.size() && line_id_sets[i].contains(node)) {
                return {i + offset, -std::stoi(node->id)};
            }
        }
        return {0, 0};
    };
    std::sort(target.begin(), target.end(), [&](const auto &a, const auto &b) {
        auto[a0, a1] = get_node_priority(a);
        auto[b0, b1] = get_node_priority(b);
        if (a0 != b0) { return a0 < b0; }
        return a1 < b1;
    });
}

static void sort_by_mathjax_id(std::vector<const NodeProps *> &target) {
    std::sort(target.begin(), target.end(), [](const auto &a, const auto &b) {
        // TODO: performance issue
        return std::stoi(a->id) < std::stoi(b->id);
    });
}


std::shared_ptr<az::pen::PenGraph> SynthesisTexGenerator::generate_next(
        const LabelType &tex,
        const ShapeProvider &shape_provider,
        const std::function<void(const UCharType &)> &on_failure,
        bool force_mathjax,
        TraverseOrder traverse_order,
        bool need_extra_info
) {
    auto graph = az::tex::convert_tex_to_graph(tex);

    std::vector<LabelType> mml_stack;
    std::stack<TransformMatrix2> transform_stack;
    transform_stack.emplace(TransformMatrix2::Identity());
    std::vector<Box2> coord_stack;
    std::unordered_map<const NodeProps *, std::shared_ptr<PenItemInfo>, NodePropsPtrHasher> node_extra_map;
    std::unordered_map<const NodeProps *, Box2, NodePropsPtrHasher> id_box_map;
    std::unordered_map<const NodeProps *, TransformMatrix2, NodePropsPtrHasher> id_transform_map;
    bool on_composed = false; // annotate if dfs under a composed char root
    Box2 composed_box;
    std::vector<const NodeProps *> composed_items;
    const auto on_discover_node = [&](const NodeProps &node) {
        // on_discover_node
        if (!node.mml.empty()) {
            mml_stack.push_back(node.mml);
        }
        auto &prev_transform = transform_stack.top();
        transform_stack.emplace(transform_stack.top() * node.transform);
        auto &current_transform = transform_stack.top();
        id_transform_map.insert({&node, current_transform});
        bool maybe_composed = "g" == node.svg_type && node.label.empty()
                              && 1 == az::js::QjsWrapper::count_unicode(node.text);
        if (maybe_composed) { // maybe a composed char
#ifdef XGD_DEBUG
            SPDLOG_INFO("start {}", node.text);
#endif
            on_composed = true;
        } else if ("svg" == node.svg_type && node.width > 0 && node.height > 0) { // handle svg root node
            coord_stack.emplace_back(
                    Vec2{node.x, node.y},
                    Vec2{node.x + node.width, node.y + node.height}
            );
        } else if ("rect" == node.svg_type) { // handle frac
            Box2 orig_box(Vec2{node.x, node.y}, Vec2{node.x + node.width, node.y + node.height});
            Transform2 trans;
            trans.matrix() = current_transform;
            auto box = orig_box.transformed(trans);
            id_box_map.insert(std::make_pair(&node, std::move(box)));
        } else if ("line" == node.svg_type) { // handle hline
            Vec2 from{node.x1, node.y1};
            Vec2 to{node.x2, node.y2};
            Box2 orig_box{from, to};
            auto offset = get_distance(from, to) * ScalarType{0.01};
            offset = std::clamp(offset, ScalarType{4}, ScalarType{30});
            auto[tl, bl] = move_line_vertically(from, to, offset);
            auto[tr, br] = move_line_vertically(from, to, -offset);
            orig_box.extend(tl);
            orig_box.extend(br);
            Transform2 trans;
            trans.matrix() = current_transform;
            auto box = orig_box.transformed(trans);
            id_box_map.insert(std::make_pair(&node, std::move(box)));
        } else if ("text" == node.svg_type) { // handle plain text
            if (1 == az::js::QjsWrapper::count_unicode(node.text)) {
                ScalarType width = node.font_size;
                Box2 box(Vec2{0, -width}, Vec2{width, 0});
                // ignore below. we use origin box to create font, apply transformation on it further.
//            Transform2 trans;
//            trans.matrix() = current_transform;
//            box.transform(trans);
                id_box_map.insert(std::make_pair(&node, std::move(box)));
            } else {
                SPDLOG_ERROR("unexpected multi unicode text in {}", node.text);
            }
        } else if (node.path) { // handle svg path
            auto box = graph.bbox(node, current_transform);
            if (!box.isEmpty()) {
                // skip root svg
                if (coord_stack.size() >= 2) {
                    auto coord_box = coord_stack.back();
                    auto tl = coord_box.min();
                    Transform2 trans;
                    trans.matrix() = prev_transform;
                    coord_box.transform(trans);
                    box = box.translated(-tl).clamp(coord_box);
                }
                if (on_composed) {
                    composed_box.extend(box);
                    composed_items.push_back(&node);
                }
                id_box_map.insert(std::make_pair(&node, std::move(box)));
            }
        }
#ifdef XGD_DEBUG
        UCharType uchar_label = convert_unicode_hex_to_str(fmt::format("0x{}", node.label));
        SPDLOG_INFO("node={},{},{},{},{}", node.svg_type, node.mml, node.label, node.text, uchar_label);
#endif
    };
    const auto on_finish_node = [&](const NodeProps &node) {
        // on_finish_node
        if (need_extra_info) {
            auto info = std::make_shared<PenItemInfo>();
            info->mml_stack = mml_stack;
            node_extra_map[&node] = info;
        }
        if (!node.mml.empty()) { mml_stack.pop_back(); }
        bool maybe_composed = "g" == node.svg_type && node.label.empty()
                              && 1 == az::js::QjsWrapper::count_unicode(node.text);
        if (on_composed && maybe_composed) { // maybe a composed char
#ifdef XGD_DEBUG
            SPDLOG_INFO("finish {}", node.text);
#endif
            // we only treat root with more than 2 items as composed item root
            if (!force_mathjax && !composed_box.isEmpty() && composed_items.size() > 1) {
                for (auto &item: composed_items) {
                    id_box_map.erase(item); // erase single items
                }
                id_box_map.insert(std::make_pair(&node, composed_box)); // insert a composed item
            }
            composed_box.setEmpty();
            composed_items.clear();
            on_composed = false;
        } else if ("svg" == node.svg_type && node.width > 0 && node.height > 0) {
            coord_stack.pop_back();
        }
        transform_stack.pop();
    };

    graph.traverse(on_discover_node, on_finish_node);
    auto pen_graph = std::make_shared<PenGraph>();

    const auto on_hw_draw_char = [&](
            const NodeProps &node,
            const UCharType &label,
            const Box2 &box,
            bool keep_ratio = true
    ) -> std::shared_ptr<PenChar> {
        LabelType origin_label = label;
        LabelType fallback_label;
        // 1. origin label is always welcome
        auto maybe_hw_char = dynamic_cast<PenChar *>(shape_provider(label));
        if (!maybe_hw_char) {
            auto maybe_label_shape = az::data::map_unicode_by_shape(label);
            if (!maybe_label_shape) {
                return nullptr;
            }
            auto &label_shape = maybe_label_shape.value();
            fallback_label = label_shape;
            // 2. try using clustered characters with similar shape
            maybe_hw_char = dynamic_cast<PenChar *>(shape_provider(label_shape));
        }
        if (!maybe_hw_char) {
            return nullptr;
        }
        // deep copy here
        auto hw_char = std::make_shared<PenChar>(*maybe_hw_char);
        auto size = box.sizes();
        if (keep_ratio) {
            hw_char->fit_into_keep_ratio(size.x(), size.y());
        } else {
            hw_char->fit_into(size.x(), size.y());
        }
        hw_char->move_center_to(box.center());
        if (need_extra_info) {
            node_extra_map[&node]->origin_label = std::move(origin_label);
            if (!fallback_label.empty()) {
                node_extra_map[&node]->fallback_label = std::move(fallback_label);
            }
        }
        return hw_char;
    };
    const auto on_hw_draw_node = [&](const NodeProps &node, const Box2 &box) -> std::shared_ptr<PenOp> {
        if (node.label.empty()) {
            if (node.data_frame && "rect" == node.svg_type) {
                return nullptr; // fall back to svg
            } else if ("rect" == node.svg_type || "line" == node.svg_type) { // handle frac
                return on_hw_draw_char(node, "-", box, false);
            } else if ("text" == node.svg_type) { // handle plain text
                Transform2 trans;
                trans.matrix() = id_transform_map[&node];
                return on_hw_draw_char(node, node.text, box.transformed(trans));
            } else if (!node.text.empty()) { // treat composed char as a common path
                return on_hw_draw_char(node, node.text, box, false);
            }
            return nullptr;
        }
        UCharType label = convert_unicode_hex_to_str(fmt::format("0x{}", node.label));
        return on_hw_draw_char(node, label, box);
    };
    const auto on_svg_draw_node = [&](const NodeProps &node, const Box2 &box) -> std::shared_ptr<PenOp> {
        if (node.data_frame && "rect" == node.svg_type) {
            auto pen_rect = std::make_shared<PenGraph>();
            std::array<Vec2, 4> points = {
                    box.corner(Box2::TopLeft),
                    box.corner(Box2::TopRight),
                    box.corner(Box2::BottomRight),
                    box.corner(Box2::BottomLeft),
            };
            for (size_t i = 0; i < points.size(); i++) {
                auto &from = points[i];
                auto &to = points[(i + 1) % points.size()];
                auto offset = get_distance(from, to) * ScalarType{0.01};
                offset = std::clamp(offset, ScalarType{4}, ScalarType{30});
                auto[tl, bl] = move_line_vertically(from, to, offset);
                auto[tr, br] = move_line_vertically(from, to, -offset);
                auto pen_line = std::make_shared<PenPath>(fmt::format(
                        "M{} {}L{} {}L{} {}L{} {}Z",
                        tl.x(), tl.y(),
                        tr.x(), tr.y(),
                        br.x(), br.y(),
                        bl.x(), bl.y()
                ));
                pen_rect->data.push_back(pen_line);
            }
            pen_rect->move_center_to(box.center());
            if (need_extra_info) {
                node_extra_map[&node]->origin_label = "□";
            }
            return pen_rect;
        } else if ("rect" == node.svg_type || "line" == node.svg_type) { // handle frac
#ifdef XGD_DEBUG
            SPDLOG_INFO("on_svg_draw_node: rect");
#endif
            auto tl = box.corner(Box2::TopLeft);
            auto tr = box.corner(Box2::TopRight);
            auto bl = box.corner(Box2::BottomLeft);
            auto br = box.corner(Box2::BottomRight);
            auto pen_line = std::make_shared<PenPath>(fmt::format(
                    "M{} {}L{} {}L{} {}L{} {}Z",
                    tl.x(), tl.y(),
                    tr.x(), tr.y(),
                    br.x(), br.y(),
                    bl.x(), bl.y()
            ));
            pen_line->move_center_to(box.center());
            if (need_extra_info) {
                node_extra_map[&node]->origin_label = "-";
            }
            return pen_line;
        } else if ("text" == node.svg_type && !node.text.empty()) { // handle svg text
            if (need_extra_info) {
                node_extra_map[&node]->origin_label = node.text;
            }
#ifdef XGD_DEBUG
            SPDLOG_INFO("on_svg_draw_node: text: {}", node.text);
#endif
            auto maybe_svg_font = dynamic_cast<PenGraph *>(shape_provider(node.text));
            if (maybe_svg_font) {
                auto pen_graph = std::make_shared<PenGraph>(*maybe_svg_font);
                auto size = box.sizes();
                pen_graph->fit_into_keep_ratio(size.x(), size.y());
                pen_graph->move_center_to(box.center());
                pen_graph->apply_transform(id_transform_map[&node]);
                return pen_graph;
            }
            auto pen_font = std::make_shared<PenFont>(node.text, box, id_transform_map[&node]);
            return pen_font;
        } else if (node.path) { // handle svg path
            if (need_extra_info) {
                UCharType label = convert_unicode_hex_to_str(fmt::format("0x{}", node.label));
                node_extra_map[&node]->origin_label = label;
            }
#ifdef XGD_DEBUG
            SPDLOG_INFO("on_svg_draw_node: path: {}", node.label);
#endif
            auto pen_path = std::make_shared<PenPath>(node.path, id_transform_map[&node]);
            auto size = box.sizes();
            pen_path->fit_into(size.x(), size.y());
            pen_path->move_center_to(box.center());
            return pen_path;
        }
        return nullptr;
    };
    std::vector<const NodeProps *> ordered_nodes;
    ordered_nodes.reserve(id_box_map.size());
    for (auto &[node_ptr, box]: id_box_map) {
        ordered_nodes.push_back(node_ptr);
    }
    switch (traverse_order) {
        case SORT_BY_MULTIPLY_DEMONSTRATION:
            sort_by_multiply_demonstration(ordered_nodes, graph);
            break;
        case DEFAULT:
        default:
            sort_by_mathjax_id(ordered_nodes);
    }
    for (auto &node_ptr: ordered_nodes) {
        auto &box = id_box_map[node_ptr];
        auto &node = *node_ptr;
        std::shared_ptr<PenOp> op = on_hw_draw_node(node, box);
        if (!op) {
            // draw svg path as a fallback
            op = on_svg_draw_node(node, box);
        }
        if (!op) {
            if (!node.text.empty()) {
                on_failure(node.text);
            }
            if (!node.label.empty()) {
                UCharType label = convert_unicode_hex_to_str(fmt::format("0x{}", node.label));
                on_failure(label);
            }
            // fail
            continue;
        }
        // success
        pen_graph->data.push_back(std::move(op));
        if (need_extra_info) {
            pen_graph->extra.push_back(node_extra_map[&node]);
        }
    }
    return pen_graph;
}

std::shared_ptr<az::pen::PenGraph> SynthesisTexGenerator::generate_sub_string(
        const LabelType &tex,
        const ShapeProvider &shape_provider
) {
    return SynthesisTexGenerator::generate_next(
            tex,
            shape_provider,
            [](auto &&) {},
            false,
            SynthesisTexGenerator::TraverseOrder::DEFAULT,
            false
    );
}
