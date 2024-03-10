#include "graph_impl.h"

#include <boost/graph/graphviz.hpp>
//#include <boost/container/flat_map.hpp>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>

#include <ranges>

using namespace boost;
using namespace az::chem;
using namespace az::chem::impl;

namespace {
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
        void black_target(Edge e, Graph &g) {
            on_edge(g[e]);
            Base::black_target(e, g);
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

void GraphImpl::bfs_traverse(
        const std::function<void(const Atom &)> &on_discover_node,
        const std::function<void(const Bond &)> &on_edge) const {
    using namespace boost;
    const auto &G = *this;
    if (G.vertex_set().empty()) {
        SPDLOG_ERROR("empty graph");
        return;
    }
    using vertex_type = graph_traits<GraphImpl>::vertex_descriptor;
    std::map<vertex_type, int> subclusters;
    boost::connected_components(G, boost::make_assoc_property_map(subclusters));
    std::unordered_set<int> cid_set;
    for (auto&[v, cid]: subclusters) {
        if (cid_set.contains(cid)) { continue; }
        cid_set.insert(cid);
        breadth_first_search(
                G,
                v,
                visitor(
                        bfs_graph_impl_visitor_function(
                                on_discover_node,
                                on_edge,
                                null_visitor()
                        )
                )
        );
    }
}
