#pragma once

#include "az/pen/config.h"
#include "az/core/config.h"

#include <optional>
#include <stack>
#include <string>

namespace az::pen {
    struct AZPEN_EXPORT PenItemInfo {
        std::optional<std::vector<LabelType>> mml_stack;
        std::optional<std::vector<LabelType>> atom_pair;
        LabelType origin_label;
        std::optional<LabelType> fallback_label;
    };
}