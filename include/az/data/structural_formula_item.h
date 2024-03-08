#pragma once

#include "az/data/config.h"
#include "az/core/utils.h"
#include "az/pen/pen_graph.h"
#include "az/data/pen_bond_factory.h"

#include <string_view>

namespace az::chem {
    class MolGraph;
}
namespace az::data {
    struct AZDATA_EXPORT StructuralFormulaItem {
        // we do not want to expose chem module as public dependency of data
        std::shared_ptr<az::chem::MolGraph> mol_graph;
        // for further compositions
        std::shared_ptr<az::pen::PenGraph> data = std::make_shared<az::pen::PenGraph>();

        StructuralFormulaItem();

        void create_from_format(
                const ShapeProvider &shape_provider,
                std::string_view input,
                std::string_view format = "smi",
                bool force_show_carbon = false
        );
    };
}
