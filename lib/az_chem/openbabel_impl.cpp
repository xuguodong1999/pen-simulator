#include "openbabel_impl.h"
#include "az/spdstream.h"

#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/ring.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel/op.h>
#include <openbabel/generic.h>
#include <openbabel/stereo/stereo.h>
#include <openbabel/stereo/cistrans.h>
#include <openbabel/stereo/tetrahedral.h>
#include <openbabel/stereo/squareplanar.h>
#include <openbabel/obfunctions.h>

#include <spdlog/fmt/fmt.h>
#include <sstream>

using namespace az;
using namespace az::chem;
using namespace az::chem::impl;

using id_to_atom_map_type = std::unordered_map<IdType, OpenBabel::OBAtom *>;
using id_to_bond_map_type = std::unordered_map<IdType, OpenBabel::OBBond *>;

//#define XGD_DEBUG

#define OB_LOG_BEGIN \
    OpenBabel::obErrorLog.SetOutputLevel(OpenBabel::obError);  \
    az::SpdStream spdlog_out{az::SpdStreamTag::CERR}; \
    OpenBabel::obErrorLog.SetOutputStream(&spdlog_out);

#define OB_LOG_END \
    std::endl(spdlog_out); \
    OpenBabel::obErrorLog.SetOutputLevel(OpenBabel::obWarning); \
    OpenBabel::obErrorLog.SetOutputStream(&std::clog);

static void openbabel_generate_3d_coords(OpenBabel::OBMol &mol) {
    auto op = OpenBabel::OBOp::FindType("gen3D");
    if (!op) {
        throw std::runtime_error("openbabel ops gen3D not found");
    }
    op->Do(&mol, "3");
}

static void openbabel_generate_2d_coords(OpenBabel::OBMol &mol) {
    auto op = OpenBabel::OBOp::FindType("2D");
    if (!op) {
        throw std::runtime_error("openbabel ops 2d not found");
    }
    op->Do(&mol);
}

static void openbabel_read_format(OpenBabel::OBMol &mol, const char *input, const char *format) {
    auto ob_format = OpenBabel::OBConversion::FindFormat(format);
    OpenBabel::OBConversion conv;
    if (!ob_format || !conv.SetInFormat(ob_format)) {
        throw std::runtime_error(fmt::format("openbabel: failed to find format {}", format));
    }
    std::istringstream in(input);
    if (!conv.Read(&mol, &in)) {
        throw std::runtime_error(fmt::format("openbabel: failed to read {} as {}", input, format));
    }
}

static std::string openbabel_write_format(OpenBabel::OBMol &mol, const char *format) {
    auto ob_format = OpenBabel::OBConversion::FindFormat(format);
    OpenBabel::OBConversion conv;
    if (!ob_format || !conv.SetOutFormat(ob_format)) {
        throw std::runtime_error(fmt::format("openbabel: failed to find format {}", format));
    }
    if (mol.Empty()) {
        throw std::runtime_error(fmt::format("openbabel: failed to save as {}", format));
    }
    return conv.WriteString(&mol, true);
}

/**
 * openbabel has a very complex way of determine stereos,
 *  we have to construct another stereo and call operator== to determine a final R/S Z/E
 * Not supported yet.
 */
static void debug_openbabel_stereo(OpenBabel::OBMol &mol) {
    OpenBabel::OBStereoFacade stereo_facade(&mol);
    for (auto &stereo: stereo_facade.GetAllCisTransStereo()) { // Z/E
        auto cfg = stereo->GetConfig();
        auto ob_bond_begin = mol.GetBondById(cfg.begin);
        auto ob_bond_end = mol.GetBondById(cfg.end);
        bool is_z = stereo->IsTrans(ob_bond_begin->GetId(), ob_bond_end->GetId());
        bool is_e = stereo->IsCis(ob_bond_begin->GetId(), ob_bond_end->GetId());
        SPDLOG_INFO("Z={}, R={}, is specified={}", is_z, is_e, cfg.specified);
    }
    for (auto &stereo: stereo_facade.GetAllSquarePlanarStereo()) { // square plane
    }
    for (auto &stereo: stereo_facade.GetAllTetrahedralStereo()) { // R/S
        auto cfg = stereo->GetConfig();
        auto ob_atom_center = mol.GetAtomById(cfg.center);
        auto ob_atom_begin = mol.GetAtomById(cfg.from);
        auto ob_atom_end = mol.GetAtomById(cfg.towards);
        bool is_r = cfg.winding == OpenBabel::OBStereo::Winding::Clockwise;
        bool is_s = cfg.winding == OpenBabel::OBStereo::Winding::AntiClockwise;
        bool direction = cfg.view == OpenBabel::OBStereo::View::ViewFrom;
        std::stringstream out;
        for (auto &ref: cfg.refs) {
            out << ref << ",";
        }
        SPDLOG_INFO("R={}, S={}, direction={}, is specified={}, from={}, to={}, center={}, chain={}",
                    is_r, is_s, direction, cfg.specified,
                    ob_atom_begin->GetAtomicNum(),
                    ob_atom_end->GetAtomicNum(),
                    ob_atom_center->GetAtomicNum(),
                    out.str());
    }
}

static void sync_from_ob_mol(
        GraphImpl &g,
        OpenBabel::OBMol &mol
) {
    using namespace boost;
    IdType base_id = 0;
    g.clear();
    std::unordered_map<unsigned long, graph_traits<GraphImpl>::vertex_descriptor> id_vec_map;

    FOR_ATOMS_OF_MOL(ob_atom_it, const_cast<OpenBabel::OBMol *>(&mol)) {
        auto ob_atom_id = ob_atom_it->GetId();
        Atom atom{
                .id = base_id++,
                .atomic_number = static_cast<uint8_t>(ob_atom_it->GetAtomicNum()),
                .charge = static_cast<int8_t>(ob_atom_it->GetFormalCharge()),
                .center_2d = Vec2{ob_atom_it->x(), ob_atom_it->y()}
        };
        auto v = add_vertex(atom, g);
        auto [vd, success] = id_vec_map.insert({ob_atom_id, v});
        if (!success) {
            SPDLOG_ERROR("failed to insert openbabel atom: {} as atom: {}", ob_atom_id, atom.id);
        }
    }
    std::map<OpenBabel::OBBond *, OpenBabel::OBStereo::BondDirection> updown;
    std::map<OpenBabel::OBBond *, OpenBabel::OBStereo::Ref> stereo_centers;
    // FIXME: this API did not distinguish between N[C@H](C(=O)O)C and N[C@@H](C(=O)O)C
    OpenBabel::TetStereoToWedgeHash(mol, updown, stereo_centers);
    FOR_BONDS_OF_MOL(ob_bond_it, const_cast<OpenBabel::OBMol *>(&mol)) {
        auto ob_bond_id = ob_bond_it->GetId();
        auto ob_from_id = ob_bond_it->GetBeginAtom()->GetId();
        auto ob_to_id = ob_bond_it->GetEndAtom()->GetId();
        auto ob_bond_order = ob_bond_it->GetBondOrder();
        BondType bond_type;
        auto stereo = OpenBabel::OBStereo::BondDirection::NotStereo;
        bool need_reverse_bond = false;
        {
            auto it = updown.find(ob_bond_it.operator->());
            if (updown.end() != it) { stereo = it->second; }
        }
        {
            auto it = stereo_centers.find(ob_bond_it.operator->());
            if (stereo_centers.end() != it) {
                need_reverse_bond = (it->second != ob_from_id);
            }
        }
        if (5 == ob_bond_order) {
            bond_type = BondType::Delocalized;
//        } else if (ob_bond_it->IsWedge()) {
        } else if (stereo == OpenBabel::OBStereo::UpBond) {
#ifdef XGD_DEBUG
            SPDLOG_INFO("up bond");
#endif
            bond_type = BondType::SolidWedge;
//        } else if (ob_bond_it->IsHash()) {
        } else if (stereo == OpenBabel::OBStereo::DownBond) {
#ifdef XGD_DEBUG
            SPDLOG_INFO("down bond");
#endif
            bond_type = BondType::DashWedge;
//        } else if (ob_bond_it->IsWedgeOrHash()) {
        } else if (stereo == OpenBabel::OBStereo::UnknownDir) {
            bond_type = BondType::Wave;
        } else if (3 == ob_bond_order) {
            bond_type = BondType::Triple;
        } else if (2 == ob_bond_order) {
            bond_type = BondType::Double;
        } else if (1 == ob_bond_order) {
            bond_type = BondType::Single;
        } else {
            SPDLOG_ERROR("unhandled openbabel bond order: {}", ob_bond_order);
        }
        if (need_reverse_bond) {
            std::swap(ob_from_id, ob_to_id);
        }
        auto &from_v = id_vec_map[ob_from_id];
        auto &to_v = id_vec_map[ob_to_id];
        auto &from = g[from_v];
        auto &to = g[to_v];
        Bond bond{
                .id=base_id++,
                .type=bond_type,
                .from=&from,
                .to=&to
        };
        auto [e, success] = add_edge(from_v, to_v, bond, g);
        if (!success) {
            SPDLOG_ERROR("failed to insert openbabel bond: {} as bond: {}", ob_bond_id, bond.id);
        }
    }
}

static void sync_to_ob_mol(
        const GraphImpl &g,
        OpenBabel::OBMol &mol,
        id_to_atom_map_type &id_to_atom_map
) {
    using namespace boost;
    mol.BeginModify();
    mol.ReserveAtoms(g.m_vertices.size());
    g.bfs_traverse([&](const Atom &a) {
        OpenBabel::OBAtom ob_atom;
        ob_atom.SetAtomicNum(a.atomic_number);
        ob_atom.SetFormalCharge(a.charge);
        ob_atom.SetId(mol.NumAtoms());
        mol.AddAtom(ob_atom, false);
        id_to_atom_map[a.id] = mol.GetAtomById(ob_atom.GetId());
    }, [&](const Bond &b) {
    });
    g.bfs_traverse([&](const Atom &a) {
    }, [&](const Bond &b) {
        OpenBabel::OBBond ob_bond;
        int order = 0;
        switch (b.type) {
            case BondType::SolidWedge:
                order = 1;
                ob_bond.SetWedge(true);
                break;
            case BondType::DashWedge:
                order = 1;
                ob_bond.SetHash(true);
                break;
            case BondType::Wave:
                order = 1;
                ob_bond.SetWedgeOrHash(true);
                break;
            case BondType::Single:
                order = 1;
                break;
            case BondType::Double:
            case BondType::DoubleZ:
            case BondType::DoubleE:
                order = 2;
                break;
            case BondType::Triple:
                order = 3;
                break;
            case BondType::Delocalized:
            case BondType::Circle:
                order = 5;
                ob_bond.SetAromatic(true);
                break;
            case BondType::None:
                order = 0;
        }
        ob_bond.SetBondOrder(order);
        ob_bond.SetId(mol.NumBonds());
        ob_bond.SetBegin(id_to_atom_map[b.from->id]);
        ob_bond.SetEnd(id_to_atom_map[b.to->id]);
        mol.AddBond(ob_bond);
    });
    g.bfs_traverse([&](const Atom &a) {
        OpenBabel::OBAtomAssignTypicalImplicitHydrogens(id_to_atom_map[a.id]);
    }, [&](const Bond &b) {
    });
    mol.EndModify();
    openbabel_generate_2d_coords(mol);
}

void az::chem::impl::openbabel::generate_graph(
        GraphImpl &g, std::string_view content, const std::string_view &format) {
    OB_LOG_BEGIN
    OpenBabel::OBMol mol;
    openbabel_read_format(mol, content.data(), format.data());
//    debug_openbabel_stereo(mol);
    sync_from_ob_mol(g, mol);
    OB_LOG_END
}

std::string az::chem::impl::openbabel::to_format(GraphImpl &g, const std::string_view &format) {
    OB_LOG_BEGIN
    OpenBabel::OBMol mol;
    id_to_atom_map_type id_to_atom_map;
    sync_to_ob_mol(g, mol, id_to_atom_map);
    auto out = openbabel_write_format(mol, format.data());
    OB_LOG_END
    return out;
}

void az::chem::impl::openbabel::generate_2d_coords(GraphImpl &g) {
    OB_LOG_BEGIN
    OpenBabel::OBMol mol;
    id_to_atom_map_type id_to_atom_map;
    sync_to_ob_mol(g, mol, id_to_atom_map);
    openbabel_generate_2d_coords(mol);
    g.bfs_traverse([&](const Atom &a) {
        auto atom = id_to_atom_map[a.id];
        (const_cast<Atom &>(a)).center_2d = Vec2{
                static_cast<ScalarType>(atom->x()),
                static_cast<ScalarType>(atom->y())
        };
    }, [&](const Bond &b) {
    });
    OB_LOG_END
}

void az::chem::impl::openbabel::generate_3d_coords(GraphImpl &g) {
    OB_LOG_BEGIN
    OpenBabel::OBMol mol;
    id_to_atom_map_type id_to_atom_map;
    sync_to_ob_mol(g, mol, id_to_atom_map);
    openbabel_generate_3d_coords(mol);
    g.bfs_traverse([&](const Atom &a) {
        auto atom = id_to_atom_map[a.id];
        (const_cast<Atom &>(a)).center_3d = Vec3{
                static_cast<ScalarType>(atom->x()),
                static_cast<ScalarType>(atom->y()),
                static_cast<ScalarType>(atom->z())
        };
    }, [&](const Bond &b) {
    });
    OB_LOG_END
}
