#pragma once

#include "path_op.h"
#include "az/core/utils.h"

#include <vector>
#include <memory>
#include <string_view>

namespace az::svg::impl::svgpp {
    PointerVec<PathOp> svgpp_parse_path(std::string_view in);
}
