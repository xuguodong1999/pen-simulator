#pragma once

#include "graph_impl.h"

/**
 * Known issue: no Z/E R/S support, use impl::rdkit instead
 */
namespace az::chem::impl::coordgenlibs {
    void generate_2d_coords(GraphImpl &g);
}
