#pragma once

#include "az/chem/config.h"
#include "az/core/config.h"
#include "az/core/geometry_def.h"
#include <cstdint>
#include <cstdlib>

namespace az::chem {
    struct AZCHEM_EXPORT Atom {
        IdType id;
        uint8_t atomic_number = 0;
        int8_t charge = 0;
        Vec2 center_2d;
        Vec3 center_3d;
        bool use_fixed_2d_coords = false;
    };

    struct AZCHEM_EXPORT AtomPtrHasher {
        std::size_t operator()(const Atom *k) const;
    };
}
