#pragma once

#include "graph_impl.h"
#include <string_view>

/**
 * Known issue: no Z/E R/S support, use impl::rdkit instead
 */
namespace az::chem::impl::openbabel {
    void generate_graph(GraphImpl &g, std::string_view content, const std::string_view &format = "smi");

    std::string to_format(GraphImpl &g, const std::string_view &format = "smi");

    void generate_2d_coords(GraphImpl &g);

    void generate_3d_coords(GraphImpl &g);

    std::string convert_format(
            const std::string_view &from_content,
            const std::string_view &from_format,
            const std::string_view &to_format
    );
}
