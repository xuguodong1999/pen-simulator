#pragma once

#include "az/core/utils.h"
#include "az/data/config.h"
#include "az/data/inkml_item.h"

namespace az::pen {
    struct PenGraph;
}

namespace az::data {
    struct AZDATA_EXPORT SynthesisTexGenerator {
        enum TraverseOrder : int32_t {
            DEFAULT = 0,
            SORT_BY_MULTIPLY_DEMONSTRATION = 1,
        };

        static std::shared_ptr<az::pen::PenGraph> generate_next(
                const LabelType &tex,
                const ShapeProvider &shape_provider,
                const std::function<void(const UCharType &)> &on_failure = [](auto &&) {},
                bool force_mathjax = false,
                TraverseOrder traverse_order = DEFAULT,
                bool need_extra_info = true
        );

        static std::shared_ptr<az::pen::PenGraph> generate_sub_string(
                const LabelType &tex,
                const ShapeProvider &shape_provider = [](auto &&) { return nullptr; }
        );
    };
}
