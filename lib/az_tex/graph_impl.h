#pragma once

#include "az/tex/api.h"
#include "az/tex/tex_graph.h"
#include "mathjax_json_io.h"

#include "az/svg/path.h"

#include <boost/graph/adjacency_list.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>

namespace az::tex::mathjax::impl {
    using namespace az::tex::mathjax;

    struct GraphProps {
        std::string tex;
        std::unordered_map<std::string, az::svg::Path> path_map;
    };

    struct GraphImpl : boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            NodeProps,
            EdgeProps,
            GraphProps,
            boost::listS> {
        void write_graphviz(std::ostream &out);

        void dfs_traverse(
                const std::function<void(const NodeProps &)> &on_discover_node,
                const std::function<void(const NodeProps &)> &on_finish_node);
    };

    struct NodeWriter {
        const GraphImpl &graph;

        explicit NodeWriter(const GraphImpl &graph) : graph(graph) {};

        template<typename V>
        void operator()(std::ostream &out, const V &index) const {
            const NodeProps &node = graph[index];
            out << " node: [\n";
            out << fmt::format("id={},\n", node.id);
            if (!node.mml.empty()) {
                out << fmt::format("mml={},\n", node.mml);
            }
            if (!node.text.empty()) {
                out << fmt::format("text={},\n", node.text);
            }
            if (!node.label.empty()) {
                out << fmt::format("label={},\n", node.label);
            }
            if (node.width > 0 && node.height > 0) {
                out << fmt::format("(x,y,w,h)=({},{},{},{}),\n", node.x, node.y, node.width, node.height);
            }
            if (node.font_size > 0) {
                out << fmt::format("font_size={},\n", node.font_size);
            }
            if (!node.transform.isIdentity()) {
                out << fmt::format("transform=[{}],\n", node.transform);
            }
            if (!node.view_box.isEmpty()) {
                out << fmt::format(
                        "view_box=[({}),({})],\n",
                        node.view_box.corner(Box2::BottomLeft),
                        node.view_box.corner(Box2::TopRight));
            }
            if (!node.path_id.empty()) {
                out << fmt::format("path_id={},\n", node.path_id);
            }
            if (!node.svg_type.empty()) {
                out << fmt::format("svg_type={},\n", node.svg_type);
            }
            out << "]";
        }
    };

    struct EdgeWriter {
        const GraphImpl &graph;

        explicit EdgeWriter(const GraphImpl &graph) : graph(graph) {};

        template<typename E>
        void operator()(std::ostream &out, const E &index) const {
            const EdgeProps &edge = graph[index];
            auto sv = source(index, graph);
            const NodeProps &sp = graph[sv];
            auto tv = target(index, graph);
            const NodeProps &tp = graph[tv];
            out << fmt::format("[{}->{}]", sp.id, tp.id);
        }
    };

    struct GraphImplWriter {
        GraphProps *prop;

        explicit GraphImplWriter(GraphProps *prop) : prop(prop) {}

        void operator()(std::ostream &out) const {
            if (!prop) { return; }
            const GraphProps &graph = *prop;
            out << fmt::format("graph: [tex={}]\n", graph.tex);
        }
    };
}