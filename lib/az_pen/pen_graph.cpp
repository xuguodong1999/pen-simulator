#include "az/pen/pen_graph.h"
#include "az/pen/pen_font.h"
#include "az/pen/pen_path.h"
#include "az/pen/pen_string.h"
#include "az/pen/pen_context.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace az;
using namespace az::pen;

PenGraph::PenGraph(const PenGraph &other) {
    *this = other;
}

PenGraph &PenGraph::operator=(const PenGraph &other) {
    if (this == &other) { return *this; }
    this->trans = other.trans;
    this->bounding_box = other.bounding_box;
    this->context = other.context;
    this->data.reserve(other.data.size());
    for (auto &item: other.data) {
        std::shared_ptr<PenOp> pen_op;
        if (auto pen_char = std::dynamic_pointer_cast<PenChar>(item)) {
            pen_op = pen_char;
        } else if (auto pen_font = std::dynamic_pointer_cast<PenFont>(item)) {
            pen_op = pen_font;
        } else if (auto pen_graph = std::dynamic_pointer_cast<PenGraph>(item)) {
            pen_op = pen_graph;
        } else if (auto pen_path = std::dynamic_pointer_cast<PenPath>(item)) {
            pen_op = pen_path;
        } else if (auto pen_stroke = std::dynamic_pointer_cast<PenStroke>(item)) {
            pen_op = pen_stroke;
        } else if (auto pen_str = std::dynamic_pointer_cast<PenString>(item)) {
            pen_op = pen_str;
        } else {
            throw std::runtime_error("unimplemented pen type copy");
        }
        this->data.push_back(pen_op);
    }
    return *this;
}

void PenGraph::rotate_skeleton(ScalarType radians, const Vec2 &center) {
    this->rotate(radians, center);
    const auto neg_radians = -radians;
    for (auto &item: this->data) {
        item->rotate(neg_radians);
    }
}

Box2 PenGraph::bbox(const TransformMatrix2 &parent_trans) {
    Box2 new_box;
    const auto current_trans = parent_trans * this->trans;
    for (auto &item: this->data) {
        new_box.extend(item->bbox(current_trans));
    }
    return new_box;
}

void PenGraph::on_paint(const TransformMatrix2 &parent_trans) {
    if (!this->context) {
        throw std::runtime_error("context is not registered in PenGraph");
    }
    const auto current_trans = parent_trans * this->trans;
    bool notify_info = this->extra.size() == this->data.size();
    for (size_t i = 0; i < this->data.size(); i++) {
        auto &item = this->data[i];
        item->set_context(this->context);
        if (notify_info) {
            auto &info = this->extra[i];
            if (info->mml_stack) { // all latex elements
                this->context->on_label(
                        info->origin_label,
                        info->fallback_label ? info->fallback_label.value() : info->origin_label
                );
                this->context->on_hierarchy(info->mml_stack.value(), info->origin_label);
            } else if (info->atom_pair) { // bond
                this->context->on_hierarchy(info->atom_pair.value(), info->origin_label);
            } else { // atom
                this->context->on_label(
                        info->origin_label,
                        info->fallback_label ? info->fallback_label.value() : info->origin_label
                );
            }
        }
        item->on_paint(current_trans);
    }
}

ScalarType PenGraph::get_average_item_size() {
    if (this->data.empty()) {
        return 0;
    }
    std::vector<float> size_list;
    size_list.reserve(this->data.size() * 2);
    for (auto &item: this->data) {
        auto size = (item->bbox(this->trans)).sizes();
        size_list.push_back(size.x());
        size_list.push_back(size.y());
    }
    return vector_medium(size_list);
}

Vec2 PenGraph::adjust_size_for_rendering(
        const ScalarType &ideal_item_size,
        const ScalarType &max_render_size,
        const ScalarType &padding_ratio
) {
    if (this->data.empty()) {
        return {0, 0};
    }
    auto predicted_size = this->get_average_item_size();
    this->dot(ideal_item_size / predicted_size);
    this->move_tl_to(Vec2{0, 0});
    auto size = this->PenOp::bbox().sizes();
    Vec2 rs{size.x(), size.y()};
    if (rs.x() > max_render_size) {
        rs.y() *= max_render_size / rs.x();
        rs.x() = max_render_size;
    }
    if (rs.y() > max_render_size) {
        rs.x() *= max_render_size / rs.y();
        rs.y() = max_render_size;
    }
    if (rs.x() != size.x() || rs.y() != size.y()) {
        this->fit_into_keep_ratio(rs.x(), rs.y());
    }
    rs *= padding_ratio;
    this->move_center_to(rs / 2);
    return rs;
}
