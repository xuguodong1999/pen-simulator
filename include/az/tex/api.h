#pragma once

#include "config.h"
#include "tex_graph.h"

#include <string>
#include <string_view>
#include <vector>

namespace az::tex {
    std::vector<std::string> AZTEX_EXPORT convert_tex_to_svg_batch(
            const std::vector<std::string_view> &in_list, unsigned int parallel_num);

    std::string AZTEX_EXPORT convert_tex_to_svg(std::string_view in);

    std::string AZTEX_EXPORT convert_tex_to_json(std::string_view in);

    mathjax::Graph AZTEX_EXPORT convert_tex_to_graph(std::string_view in);
}
