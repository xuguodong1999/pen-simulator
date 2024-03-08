#include "rdkit_impl.h"

#include <GraphMol/AtomIterators.h>
#include <GraphMol/BondIterators.h>
#include <GraphMol/RWMol.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/SmilesParse/SmartsWrite.h>
#include <GraphMol/FileParsers/MolFileStereochem.h>
#include <GraphMol/FileParsers/FileParsers.h>
#include <GraphMol/ForceFieldHelpers/MMFF/MMFF.h>
#include <GraphMol/DistGeomHelpers/Embedder.h>
#include <INCHI-API/inchi.h>

#include <spdlog/fmt/fmt.h>

#include <boost/algorithm/string.hpp>

#include <sstream>

using namespace az;
using namespace az::chem;
using namespace az::chem::impl;

using id_to_atom_map_type = std::unordered_map<IdType, RDKit::Atom *>;
using id_to_bond_map_type = std::unordered_map<IdType, RDKit::Bond *>;

//#define XGD_DEBUG

static void sync_from_rd_mol(
        GraphImpl &g,
        RDKit::ROMol &mol
) {
    using namespace boost;
    IdType base_id = 0;
    g.clear();
    if (!mol.getNumConformers()) {
        RDDepict::compute2DCoords(mol);
        RDKit::WedgeMolBonds(mol, &mol.getConformer());
    }
    std::unordered_map<unsigned int, graph_traits<GraphImpl>::vertex_descriptor> id_vec_map;
    for (auto rd_atom_it = mol.beginAtoms(); rd_atom_it != mol.endAtoms(); ++rd_atom_it) {
        auto rd_atom_id = (*rd_atom_it)->getIdx();
#ifdef XGD_DEBUG
        SPDLOG_INFO("read atom {}", rd_atom_id);
#endif
        Atom atom{
                .id = base_id++,
                .atomic_number = static_cast<uint8_t>((*rd_atom_it)->getAtomicNum()),
                .charge = static_cast<int8_t>((*rd_atom_it)->getFormalCharge()),
        };
        auto v = add_vertex(atom, g);
        auto [vd, success] = id_vec_map.insert({rd_atom_id, v});
        if (!success) {
            SPDLOG_ERROR("failed to insert rdkit atom: {} as atom: {}", rd_atom_id, atom.id);
        }
    }
    for (auto rd_bond_it = mol.beginBonds(); rd_bond_it != mol.endBonds(); ++rd_bond_it) {
        auto rd_bond_id = (*rd_bond_it)->getIdx();
#ifdef XGD_DEBUG
        SPDLOG_INFO("read bond {}", rd_bond_id);
#endif
        auto rd_from_id = (*rd_bond_it)->getBeginAtomIdx();
        auto rd_to_id = (*rd_bond_it)->getEndAtomIdx();
        auto rd_bond_order = (*rd_bond_it)->getBondType();
        auto rd_bond_dir = (*rd_bond_it)->getBondDir();
        auto rd_bond_stereo = (*rd_bond_it)->getStereo();
        BondType bond_type;
        switch (rd_bond_order) {
            case RDKit::Bond::BondType::AROMATIC:
                bond_type = BondType::Delocalized;
                break;
            case RDKit::Bond::BondType::SINGLE: {
                switch (rd_bond_dir) {
                    case RDKit::Bond::BondDir::BEGINDASH:
                        bond_type = BondType::DashWedge;
                        break;
                    case RDKit::Bond::BondDir::BEGINWEDGE:
                        bond_type = BondType::SolidWedge;
                        break;
                    case RDKit::Bond::BondDir::UNKNOWN:
                        bond_type = BondType::Wave;
                        break;
                    default:
                        bond_type = BondType::Single;
                }
                break;
            }
            case RDKit::Bond::BondType::DOUBLE:
                switch (rd_bond_stereo) {
                    case RDKit::Bond::BondStereo::STEREOE :
                        bond_type = BondType::DoubleE;
                        break;
                    case RDKit::Bond::BondStereo::STEREOZ :
                        bond_type = BondType::DoubleZ;
                        break;
                    default:
                        bond_type = BondType::Double;
                }
                break;
            case RDKit::Bond::BondType::TRIPLE:
                bond_type = BondType::Triple;
                break;
            default:
                SPDLOG_ERROR("unhandled openbabel bond order: {}", rd_bond_order);
        }
        auto &from_v = id_vec_map[rd_from_id];
        auto &to_v = id_vec_map[rd_to_id];
        auto &from = g[from_v];
        auto &to = g[to_v];
        Bond bond{
                .id=base_id++,
                .type=bond_type,
                .from=&from,
                .to=&to
        };
        auto &rd_stereo_atoms = (*rd_bond_it)->getStereoAtoms();
        bond.stereo_atoms.reserve(rd_stereo_atoms.size());
        for (auto &rd_atom_id: rd_stereo_atoms) {
            bond.stereo_atoms.push_back(&(g[id_vec_map[rd_atom_id]]));
        }
        auto [e, success] = add_edge(from_v, to_v, bond, g);
        if (!success) {
            SPDLOG_ERROR("failed to insert openbabel bond: {} as bond: {}", rd_bond_id, bond.id);
        }
    }
//    mol.debugMol(std::cout);
}

static void sync_to_rd_mol(
        const GraphImpl &g,
        RDKit::RWMol &mol,
        id_to_atom_map_type &id_to_atom_map
) {
    using namespace boost;
    g.bfs_traverse([&](const Atom &a) {
#ifdef XGD_DEBUG
        SPDLOG_INFO("handle atom {}", a.id);
#endif
        RDKit::Atom rd_atom;
        rd_atom.setAtomicNum(a.atomic_number);
        rd_atom.setFormalCharge(a.charge);
        rd_atom.setIdx(mol.getNumAtoms());
        auto rd_atom_id = mol.addAtom(&rd_atom, false, false);
        id_to_atom_map[a.id] = mol.getAtomWithIdx(rd_atom_id);
    }, [&](const Bond &b) {
    });
    g.bfs_traverse([&](const Atom &a) {
    }, [&](const Bond &b) {
#ifdef XGD_DEBUG
        SPDLOG_INFO("handle bond {}", b.id);
#endif
        auto rd_bond_type = RDKit::Bond::BondType::SINGLE;
        auto rd_bond_dir = RDKit::Bond::BondDir::NONE;
        auto rd_bond_stereo = RDKit::Bond::BondStereo::STEREONONE;
        switch (b.type) {
            case BondType::SolidWedge:
                rd_bond_dir = RDKit::Bond::BondDir::BEGINWEDGE;
                break;
            case BondType::DashWedge:
                rd_bond_dir = RDKit::Bond::BondDir::BEGINDASH;
                break;
            case BondType::Wave:
                rd_bond_dir = RDKit::Bond::BondDir::UNKNOWN;
                break;
            case BondType::Single:
                break;
            case BondType::Double:
                rd_bond_type = RDKit::Bond::BondType::DOUBLE;
                break;
            case BondType::DoubleZ:
                rd_bond_type = RDKit::Bond::BondType::DOUBLE;
                rd_bond_stereo = RDKit::Bond::BondStereo::STEREOZ;
                break;
            case BondType::DoubleE:
                rd_bond_type = RDKit::Bond::BondType::DOUBLE;
                rd_bond_stereo = RDKit::Bond::BondStereo::STEREOE;
                break;
            case BondType::Triple:
                rd_bond_type = RDKit::Bond::BondType::TRIPLE;
                break;
            case BondType::Delocalized:
            case BondType::Circle:
                rd_bond_type = RDKit::Bond::BondType::AROMATIC;
                break;
            case BondType::None:
            default:
                throw std::runtime_error(fmt::format("unhandled bond type: {}", (uint8_t) b.type));
        }
        mol.addBond(id_to_atom_map[b.from->id], id_to_atom_map[b.to->id], rd_bond_type);
        auto rd_bond = mol.getBondBetweenAtoms(
                id_to_atom_map[b.from->id]->getIdx(),
                id_to_atom_map[b.to->id]->getIdx()
        );
        rd_bond->setBondDir(rd_bond_dir);
        rd_bond->setStereo(rd_bond_stereo);
    });
    g.bfs_traverse([&](const Atom &a) {
    }, [&](const Bond &b) {
        if (2 != b.stereo_atoms.size()) { return; }
        auto rd_bond = mol.getBondBetweenAtoms(
                id_to_atom_map[b.from->id]->getIdx(),
                id_to_atom_map[b.to->id]->getIdx()
        );
        rd_bond->setStereoAtoms(
                id_to_atom_map[b.stereo_atoms[0]->id]->getIdx(),
                id_to_atom_map[b.stereo_atoms[1]->id]->getIdx()
        );
    });
    RDKit::MolOps::sanitizeMol(mol);
    RDDepict::compute2DCoords(mol);
    RDKit::MolOps::detectBondStereochemistry(mol);
    RDKit::MolOps::assignChiralTypesFromBondDirs(mol);
    RDKit::MolOps::assignStereochemistry(mol);
//    mol.debugMol(std::cout);
}

static std::unordered_set<std::string> SMILES_FORMAT = {
        "smi",
        "smiles",
        "can",
        "canonical",
};

void az::chem::impl::rdkit::generate_graph(
        GraphImpl &g, std::string_view content, const std::string_view &format) {
    std::string format_lower = format.data();
    boost::algorithm::to_lower(format_lower);
    std::unique_ptr<RDKit::RWMol> mol;
    if (SMILES_FORMAT.contains(format_lower)) {
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::SmilesToMol(content.data()));
    } else if ("smarts" == format_lower) {
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::SmartsToMol(content.data()));
    } else if ("mdl" == format_lower) {
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::MolBlockToMol(content.data()));
    } else if ("mol2" == format_lower) {
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::Mol2BlockToMol(content.data()));
    } else if ("inchi" == format_lower) {
        RDKit::ExtraInchiReturnValues rv;
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::InchiToMol(content.data(), rv));
    } else if ("pdb" == format_lower) {
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::PDBBlockToMol(content.data()));
    } else if ("xyz" == format_lower) {
        mol = std::unique_ptr<RDKit::RWMol>(RDKit::XYZBlockToMol(content.data()));
    } else {
        throw std::runtime_error(fmt::format("{} is not supported in rdkit impl, try openbabel impl instead", format));
    }
    // make below codes exception safe until free of mol
    if (nullptr == mol) {
        throw std::runtime_error(fmt::format("rdkit: failed to read {} as {}", content, format));
    }
    RDKit::MolOps::Kekulize(*mol);
    sync_from_rd_mol(g, *mol);
}

std::string az::chem::impl::rdkit::to_format(GraphImpl &g, const std::string_view &format) {
    RDKit::RWMol mol;
    id_to_atom_map_type id_to_atom_map;
    sync_to_rd_mol(g, mol, id_to_atom_map);

    std::string format_lower = format.data();
    boost::algorithm::to_lower(format_lower);
    if (SMILES_FORMAT.contains(format_lower)) {
        return RDKit::MolToSmiles(mol);
    } else if ("smarts" == format_lower) {
        return RDKit::MolToSmarts(mol);
    } else if ("mdl" == format_lower) {
        return RDKit::MolToMolBlock(mol);
    } else if ("inchi" == format_lower) {
        RDKit::ExtraInchiReturnValues rv;
        return RDKit::MolToInchi(mol, rv);
    } else if ("inchikey" == format_lower) {
        return RDKit::MolToInchiKey(mol);
    } else if ("pdb" == format_lower) {
        return RDKit::MolToPDBBlock(mol);
    } else if ("xyz" == format_lower) {
        return RDKit::MolToXYZBlock(mol);
    } else {
        throw std::runtime_error(fmt::format("{} is not supported in rdkit impl, try openbabel impl instead", format));
    }
}

void az::chem::impl::rdkit::generate_2d_coords(GraphImpl &g) {
    RDKit::RWMol mol;
    id_to_atom_map_type id_to_atom_map;
    sync_to_rd_mol(g, mol, id_to_atom_map);
    RDDepict::compute2DCoords(mol);
    auto &conformer = mol.getConformer();
    g.bfs_traverse([&](const Atom &a) {
        auto atom = id_to_atom_map[a.id];
        auto &pos = conformer.getAtomPos(atom->getIdx());
#ifdef XGD_DEBUG
        SPDLOG_INFO("write {} {}", pos.x, pos.y);
#endif
        (const_cast<Atom &>(a)).center_2d = Vec2{
                static_cast<ScalarType>(pos.x),
                static_cast<ScalarType>(pos.y),
        };
    }, [&](const Bond &b) {
    });
}

void az::chem::impl::rdkit::generate_3d_coords(GraphImpl &g) {
    RDKit::RWMol mol;
    id_to_atom_map_type id_to_atom_map;
    sync_to_rd_mol(g, mol, id_to_atom_map);
    RDKit::MolOps::addHs(mol);
    RDKit::DGeomHelpers::EmbedMolecule(mol);
    RDKit::MMFF::MMFFOptimizeMolecule(mol, 1000, "MMFF94s");

    auto &conformer = mol.getConformer();
    g.bfs_traverse([&](const Atom &a) {
        auto atom = id_to_atom_map[a.id];
        auto &pos = conformer.getAtomPos(atom->getIdx());
        (const_cast<Atom &>(a)).center_3d = Vec3{
                static_cast<ScalarType>(pos.x),
                static_cast<ScalarType>(pos.y),
                static_cast<ScalarType>(pos.z),
        };
    }, [&](const Bond &b) {
    });
}
