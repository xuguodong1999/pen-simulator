#include "az/chem/mol_graph.h"

#include "graph_impl.h"
#include "rdkit_impl.h"
#include "openbabel_impl.h"
#include "coordgenlibs_impl.h"

using namespace az;
using namespace az::chem;

MolGraph::MolGraph() : p(std::make_unique<impl::GraphImpl>()) {
}

MolGraph::MolGraph(MolGraph &&g) noexcept: p(std::move(g.p)) {
}


MolGraph::~MolGraph() = default;

impl::GraphImpl *MolGraph::get_pimpl() {
    return p.get();
}

void MolGraph::traverse(
        const std::function<void(const Atom &)> &on_discover_node,
        const std::function<void(const Bond &)> &on_edge) {
    return p->bfs_traverse(on_discover_node, on_edge);
}

void MolGraph::from_format(std::string_view input, std::string_view format) {
    from_format < RDKitBackend > (input, format);
}

std::string MolGraph::to_format(std::string_view format) const {
    return to_format < RDKitBackend > (format);
}

void MolGraph::generate_2d() {
    generate_2d < RDKitBackend > ();
//    generate_2d < CoordgenlibsBackend > ();
}

void MolGraph::generate_3d() {
    generate_3d < RDKitBackend > ();
}

size_t MolGraph::bond_count() const {
    return p->m_edges.size();
}

size_t MolGraph::atom_count() const {
    return p->m_vertices.size();
}

std::string MolGraph::to_format_openbabel_impl(std::string_view format) const {
    return az::chem::impl::openbabel::to_format(*p, format);
}

std::string MolGraph::to_format_rdkit_impl(std::string_view format) const {
    return az::chem::impl::rdkit::to_format(*p, format);
}

void MolGraph::from_format_openbabel_impl(std::string_view input, std::string_view format) {
    az::chem::impl::openbabel::generate_graph(*p, input, format);
}

void MolGraph::from_format_rdkit_impl(std::string_view input, std::string_view format) {
    az::chem::impl::rdkit::generate_graph(*p, input, format);
}

void MolGraph::generate_2d_openbabel_impl() {
    az::chem::impl::openbabel::generate_2d_coords(*p);
}

void MolGraph::generate_2d_rdkit_impl() {
    az::chem::impl::rdkit::generate_2d_coords(*p);
}

void MolGraph::generate_2d_coordgenlibs_impl() {
    az::chem::impl::coordgenlibs::generate_2d_coords(*p);
}

void MolGraph::generate_3d_openbabel_impl() {
    az::chem::impl::openbabel::generate_3d_coords(*p);
}

void MolGraph::generate_3d_rdkit_impl() {
    az::chem::impl::rdkit::generate_3d_coords(*p);
}
