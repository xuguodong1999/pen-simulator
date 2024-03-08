#include "graph_impl.h"

#include <boost/graph/graphviz.hpp>
//#include <boost/container/flat_map.hpp>

#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>

#include <ranges>

using namespace boost;
using namespace az::tex::mathjax;
using namespace az::tex::mathjax::impl;

namespace {
    template<class VisitorList>
    struct dfs_graph_impl_visitor : public dfs_visitor<VisitorList> {
        using Base = dfs_visitor<VisitorList>;
        const std::function<void(const NodeProps &)> &on_discover_node;
        const std::function<void(const NodeProps &)> &on_finish_node;

        explicit dfs_graph_impl_visitor(
                const std::function<void(const NodeProps &)> &on_discover_node,
                const std::function<void(const NodeProps &)> &on_finish_node,
                const VisitorList &v = null_visitor()) :
                on_discover_node(on_discover_node),
                on_finish_node(on_finish_node),
                Base(v) {}

        template<class Vertex, class Graph>
        void discover_vertex(Vertex u, const Graph &g) {
            on_discover_node(g[u]);
            Base::discover_vertex(u, g);
        }

        template<class Vertex, class Graph>
        void finish_vertex(Vertex u, const Graph &g) {
            on_finish_node(g[u]);
            Base::finish_vertex(u, g);
        }
    };

    template<class VisitorList>
    static dfs_graph_impl_visitor<VisitorList> dfs_graph_impl_visitor_function(
            const std::function<void(const NodeProps &)> &on_discover_node,
            const std::function<void(const NodeProps &)> &on_finish_node,
            const VisitorList &v) {
        return dfs_graph_impl_visitor<VisitorList>(on_discover_node, on_finish_node, v);
    }
}

void GraphImpl::write_graphviz(std::ostream &out) {
    const auto &graph = *this;
    // required for graph with list vec type
//        boost::container::flat_map<graph_traits<GraphImpl>::vertex_descriptor, size_t> index;
//        index.reserve(num_vertices(graph));
//        // OR just: std::map<typename graph_type::vertex_descriptor, size_t> index;
//        for (size_t i = 0; auto v: boost::make_iterator_range(vertices(graph))) {
//            index[v] = i++;
//        }
    boost::write_graphviz(
            out, graph,
            NodeWriter{graph},
            EdgeWriter{graph},
            GraphImplWriter{graph.m_property.get()}
//                GraphImplWriter{graph.m_property.get()},
//                boost::make_assoc_property_map(index)
    );

}

void GraphImpl::dfs_traverse(
        const std::function<void(const NodeProps &)> &on_discover_node,
        const std::function<void(const NodeProps &)> &on_finish_node) {
    using namespace boost;
    const auto &G = *this;
    if (G.vertex_set().empty()) {
        SPDLOG_ERROR("empty graph");
        return;
    }
    using size_type = graph_traits<GraphImpl>::vertices_size_type;
    using vertex_type = graph_traits<GraphImpl>::vertex_descriptor;
    std::vector<vertex_type> result;
    topological_sort(G, std::back_inserter(result));

    std::vector<size_type> d(num_vertices(G));
    std::vector<size_type> f(num_vertices(G));
    int t = 0;
    auto index_map = boost::get(vertex_index, G);
    auto color_map = make_vector_property_map<default_color_type>(index_map);

    depth_first_search(
            G,
            visitor(
                    dfs_graph_impl_visitor_function(
                            on_discover_node,
                            on_finish_node,
                            std::make_pair(
                                    stamp_times(&d[0], t, on_discover_vertex{}),
                                    stamp_times(&f[0], t, on_finish_vertex{})
                            )
                    )
            ).color_map(color_map).root_vertex(result.back())
    );
}

namespace az::tex::mathjax::impl {
    az::svg::Path *extract_path_link(
            std::string_view sv,
            const GraphImpl &graph
    ) {
//        if (!sv.starts_with("#")) { return nullptr; }
//        sv = sv.substr(1, sv.length() - 1);
        auto &map = graph.m_property->path_map;
        auto it = map.find(sv.data());
        if (map.end() == it) {
            return nullptr;
        }
        return &it->second;
    }
}
