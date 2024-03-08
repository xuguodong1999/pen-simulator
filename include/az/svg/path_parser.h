#pragma once

#include "az/svg/config.h"
#include "az/svg/path.h"

#include <string_view>

namespace az::svg {
    AZSVG_EXPORT Path parse_path(std::string_view in);
}
