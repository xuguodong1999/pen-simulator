#pragma once

#include "az/core/geometry_def.h"
#include <string_view>

namespace az::svg::impl::svgpp {
    TransformMatrix2 parse_transform(std::string_view in);
}