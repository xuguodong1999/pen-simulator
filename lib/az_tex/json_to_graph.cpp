#include "graph_impl.h"

#include "az/svg/path_parser.h"
#include "az/svg/transform_parser.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>
#include <unordered_map>

using namespace az;

//#define XGD_DEBUG

template<>
struct fmt::formatter<Box2> {
    static constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const Box2 &bbox, FormatContext &ctx) -> decltype(ctx.out()) {
        const auto &tl = bbox.min();
        const auto &br = bbox.max();
        std::ostringstream osm;
        osm << "(" << tl.x() << "," << tl.y() << "),(" << br.x() << "," << br.y() << ")";
        return fmt::format_to(ctx.out(), "{}", osm.str());
    }
};

namespace az::tex::mathjax::impl {
    az::svg::Path *extract_path_link(std::string_view sv, const GraphImpl &graph);

    static ScalarType extract_css_size(std::string_view sv) {
        if (sv.empty()) { return 0; }
        if (sv.ends_with("px") || sv.ends_with("ex")) {
            sv = sv.substr(0, sv.length() - 2);
        }
        try {
            if constexpr (std::is_same_v<double, ScalarType>) {
                return std::stod(sv.data());
            } else {
                return std::stof(sv.data());
            }
        } catch (...) {
            SPDLOG_ERROR("unexpected css size: {}", sv);
            return 0;
        }
    }

    NodeProps convert_jsonnode_to_node(const std::string &id, const Node &json_node, const GraphImpl &graph) {
        const auto &attr = json_node.metadata.attributes;
        NodeProps node{
                .id=id,
                .svg_type=json_node.metadata.kind,
                .mml=attr.data_mml_node,
                .label=attr.data_c,
                .text=attr.data_mml_text.empty() ? json_node.metadata.value : attr.data_mml_text,

                .transform=az::svg::parse_transform(attr.transform),

                .font_size = extract_css_size(attr.font_size),
                .width = extract_css_size(attr.width),
                .height = extract_css_size(attr.height),
                .x = extract_css_size(attr.x),
                .y = extract_css_size(attr.y),
                .x1 = extract_css_size(attr.x1),
                .y1 = extract_css_size(attr.y1),
                .x2 = extract_css_size(attr.x2),
                .y2 = extract_css_size(attr.y2),
                .data_frame = (attr.data_frame == "true"),
        };
        if (attr.xlink_href.starts_with("#")) {
            node.path_id = attr.xlink_href.substr(1, attr.xlink_href.length() - 1);
        }
        if (!attr.view_box.empty()) {
            std::istringstream in(attr.view_box);
            double x0, y0, w, h;
            in >> x0 >> y0 >> w >> h;
            node.view_box.extend(Vec2{x0, y0});
            node.view_box.extend(Vec2{x0 + w, y0 + h});
        }
        return node;
    }

    EdgeProps convert_jsonedge_to_edge(const Edge &json_edge) {
        return EdgeProps{};
    }

    void convert_json_output_to_graph(const Output &json_graph, GraphImpl &graph) {
        using namespace boost;
        graph.clear();
        auto &graph_props = *graph.m_property;
        graph_props.tex = json_graph.label;
        std::unordered_map<std::string, graph_traits<GraphImpl>::vertex_descriptor> id_vec_map;
        for (auto &[id, json_node]: json_graph.nodes) {
            if ("path" == json_node.metadata.kind) {
                const auto &attr = json_node.metadata.attributes;
                if (!attr.d.empty()) {
                    auto path = az::svg::parse_path(attr.d);
#ifdef XGD_DEBUG
                    auto rect = path.bbox();
                    SPDLOG_INFO("path {} box={}", attr.id, rect);
#endif
                    graph_props.path_map.insert({attr.id, std::move(path)});
                }
            }
            auto v = add_vertex(convert_jsonnode_to_node(id, json_node, graph), graph);
            id_vec_map.insert({id, v});
        }
        for (auto &[id, v]: id_vec_map) {
            graph[v].path = extract_path_link(graph[v].path_id, graph);
        }
        for (size_t i = 0; i < json_graph.edges.size(); i++) {
            auto &json_edge = json_graph.edges[i];
            auto v0 = id_vec_map.find(json_edge.source);
            if (id_vec_map.end() == v0) {
                SPDLOG_ERROR("missing source {} in edge {}", json_edge.source, json_edge.label);
                continue;
            }
            auto v1 = id_vec_map.find(json_edge.target);
            if (id_vec_map.end() == v1) {
                SPDLOG_ERROR("missing target {} in edge {}", json_edge.target, json_edge.label);
                continue;
            }
            auto [ed, success] = add_edge(v0->second, v1->second, convert_jsonedge_to_edge(json_edge), graph);
        }
#ifdef XGD_DEBUG
        graph.write_graphviz(std::cout);
#endif
    }
}

