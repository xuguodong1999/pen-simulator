#include "graph_impl.h"

#include <boost/graph/graphviz.hpp>
//#include <boost/container/flat_map.hpp>

#include <boost/graph/topological_sort.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>

#include <ranges>

using namespace boost;
using namespace az::chem;
using namespace az::chem::impl;

namespace {
    template<class VisitorList>
    struct dfs_graph_impl_visitor : public dfs_visitor<VisitorList> {
        using Base = dfs_visitor<VisitorList>;
        const std::function<void(const Atom &)> &on_discover_node;
        const std::function<void(const Bond &)> &on_edge;

        explicit dfs_graph_impl_visitor(
                const std::function<void(const Atom &)> &on_discover_node,
                const std::function<void(const Bond &)> &on_edge,
                const VisitorList &v = null_visitor()) :
                on_discover_node(on_discover_node),
                on_edge(on_edge),
                Base(v) {}

        template<class Edge, class Graph>
        void finish_edge(Edge e, const Graph &g) {
            on_edge(g[e]);
            Base::finish_edge(e, g);
        }

        template<class Vertex, class Graph>
        void discover_vertex(Vertex u, const Graph &g) {
            on_discover_node(g[u]);
            Base::discover_vertex(u, g);
        }
    };

    template<class VisitorList>
    static dfs_graph_impl_visitor<VisitorList> dfs_graph_impl_visitor_function(
            const std::function<void(const Atom &)> &on_discover_node,
            const std::function<void(const Bond &)> &on_edge,
            const VisitorList &v) {
        return dfs_graph_impl_visitor<VisitorList>(on_discover_node, on_edge, v);
    }


    template<class VisitorList>
    struct bfs_graph_impl_visitor : public bfs_visitor<VisitorList> {
        using Base = bfs_visitor<VisitorList>;
        const std::function<void(const Atom &)> &on_discover_node;
        const std::function<void(const Bond &)> &on_edge;

        explicit bfs_graph_impl_visitor(
                const std::function<void(const Atom &)> &on_discover_node,
                const std::function<void(const Bond &)> &on_edge,
                const VisitorList &v = null_visitor()) :
                on_discover_node(on_discover_node),
                on_edge(on_edge),
                Base(v) {}

        template<class Edge, class Graph>
        void tree_edge(Edge e, Graph &g) {
            on_edge(g[e]);
            Base::tree_edge(e, g);
        }

        template<class Edge, class Graph>
        void non_tree_edge(Edge e, Graph &g) {
            on_edge(g[e]);
            Base::non_tree_edge(e, g);
        }

        template<class Vertex, class Graph>
        void discover_vertex(Vertex u, Graph &g) {
            on_discover_node(g[u]);
            Base::discover_vertex(u, g);
        }
    };

    template<class VisitorList>
    static bfs_graph_impl_visitor<VisitorList> bfs_graph_impl_visitor_function(
            const std::function<void(const Atom &)> &on_discover_node,
            const std::function<void(const Bond &)> &on_edge,
            const VisitorList &v) {
        return bfs_graph_impl_visitor<VisitorList>(on_discover_node, on_edge, v);
    }
}

void GraphImpl::dfs_traverse(
        const std::function<void(const Atom &)> &on_discover_node,
        const std::function<void(const Bond &)> &on_edge) const {
    using namespace boost;
    const auto &G = *this;
    if (G.vertex_set().empty()) {
        SPDLOG_ERROR("empty graph");
        return;
    }
    using size_type = graph_traits<GraphImpl>::vertices_size_type;

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
                            on_edge,
                            std::make_pair(
                                    stamp_times(&d[0], t, on_discover_vertex{}),
                                    stamp_times(&f[0], t, on_finish_vertex{})
                            )
                    )
            ).color_map(color_map)
    );
}

void GraphImpl::bfs_traverse(
        const std::function<void(const Atom &)> &on_discover_node,
        const std::function<void(const Bond &)> &on_edge) const {
    using namespace boost;
    const auto &G = *this;
    if (G.vertex_set().empty()) {
        SPDLOG_ERROR("empty graph");
        return;
    }
    using size_type = graph_traits<GraphImpl>::vertices_size_type;
    using vertex_type = graph_traits<GraphImpl>::vertex_descriptor;
    std::vector<vertex_type> result;
    try {
        topological_sort(G, std::back_inserter(result));
    } catch (...) {
        result = {*(G.vertex_set().begin())};
    }

    std::vector<size_type> d(num_vertices(G));
    std::vector<size_type> f(num_vertices(G));
    int t = 0;
    auto index_map = boost::get(vertex_index, G);
    auto color_map = make_vector_property_map<default_color_type>(index_map);

    breadth_first_search(
            G,
            result.back(),
            visitor(
                    bfs_graph_impl_visitor_function(
                            on_discover_node,
                            on_edge,
                            std::make_pair(
                                    stamp_times(&d[0], t, on_discover_vertex{}),
                                    stamp_times(&f[0], t, on_finish_vertex{})
                            )
                    )
            ).color_map(color_map).root_vertex(result.back())
    );
}
