#include "az/tex/tex_graph.h"
#include "graph_impl.h"

using namespace az;
using namespace az::tex::mathjax;

namespace az::tex::mathjax::impl {
    az::svg::Path *extract_path_link(std::string_view sv, const GraphImpl &graph);
}

std::size_t NodePropsPtrHasher::operator()(const NodeProps *k) const {
    return std::hash<std::string>()(k->id);
}

Graph::Graph() : p(std::make_unique<impl::GraphImpl>()) {
}

impl::GraphImpl *Graph::get_pimpl() {
    return p.get();
}

Graph::Graph(Graph &&g) noexcept: p(std::move(g.p)) {
}

void Graph::traverse(
        const std::function<void(const NodeProps &)> &on_discover_node,
        const std::function<void(const NodeProps &)> &on_finish_node) {
    // we always use dfs for svg stack, bfs make no sense here.
    p->dfs_traverse(on_discover_node, on_finish_node);
}

Box2 Graph::bbox(const NodeProps &node) const {
    auto path = extract_path_link(node.path_id, *p);
    if (!path) {
        return {};
    }
    return path->bbox();
}

Box2 Graph::bbox(const NodeProps &node, const TransformMatrix2 &mat) const {
    auto path = extract_path_link(node.path_id, *p);
    if (!path) {
        return {};
    }
    return path->bbox(mat);
}

Graph::~Graph() = default;
