#pragma once

#include "az/svg/config.h"
#include "az/core/geometry_def.h"

#include <string_view>

namespace az::svg {
    AZSVG_EXPORT TransformMatrix2 parse_transform(std::string_view in);
}