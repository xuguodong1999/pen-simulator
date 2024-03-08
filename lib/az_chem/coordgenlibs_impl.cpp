#include "coordgenlibs_impl.h"

#include <coordgen/sketcherMinimizer.h>

#include <unordered_map>

using namespace az;
using namespace az::chem::impl;

using id_to_atom_map_type = std::unordered_map<IdType, sketcherMinimizerAtom *>;

void az::chem::impl::coordgenlibs::generate_2d_coords(GraphImpl &g) {
    sketcherMinimizer minimizer;
    auto mol = new sketcherMinimizerMolecule();
    id_to_atom_map_type id_to_atom_map;
    // record atom
    g.bfs_traverse([&](const Atom &a) {
        auto atom = mol->addNewAtom();
        atom->setAtomicNumber(a.atomic_number);
        if (a.use_fixed_2d_coords) {
            atom->constrained = true;
            atom->fixed = true;
            atom->templateCoordinates = sketcherMinimizerPointF{a.center_2d.x(), a.center_2d.y()};
        }
        id_to_atom_map[a.id] = atom;
    }, [&](const Bond &b) {
    });
    // record bond
    g.bfs_traverse([&](const Atom &a) {
    }, [&](const Bond &b) {
        auto bond = mol->addNewBond(id_to_atom_map[b.from->id], id_to_atom_map[b.to->id]);
        bond->setBondOrder(frac_to_int<int>(b.valence));
    });
    // generate coords
    minimizer.initialize(mol);
    minimizer.runGenerateCoordinates();
    // record coords
    g.bfs_traverse([&](const Atom &a) {
        auto atom = id_to_atom_map[a.id];
        auto coord = atom->getCoordinates();
        (const_cast<Atom &>(a)).center_2d = Vec2{
                coord.x(),
                coord.y()
        };
    }, [&](const Bond &b) {
    });
}
