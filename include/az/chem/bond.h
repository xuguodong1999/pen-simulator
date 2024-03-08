#pragma once

#include "az/chem/config.h"
#include "az/core/config.h"
#include "az/core/fraction.h"
#include "az/core/geometry_def.h"

namespace az::chem {
    struct Atom;
    enum class AZCHEM_EXPORT BondType : uint8_t {
        None = 0,// ErrorType
        Single = 1,
        Double = 2,
        Triple = 3,
        SolidWedge,
        DashWedge,
        Wave,
        Delocalized,
        Circle, // kekule benzene ring, only for render
        DoubleZ,
        DoubleE,
    };
    struct AZCHEM_EXPORT Bond {
        IdType id;
        BondType type = BondType::None;
        Atom *from = nullptr;
        Atom *to = nullptr;
        FracType valence = 0;
        std::vector<Atom *> stereo_atoms;
    };
}