#pragma once

#include "graph_impl.h"
#include <string_view>

namespace az::chem::impl::rdkit {
    void generate_graph(GraphImpl &g, std::string_view content, const std::string_view &format = "smi");

    std::string to_format(GraphImpl &g, const std::string_view &format = "smi");

    void generate_2d_coords(GraphImpl &g);

    void generate_3d_coords(GraphImpl &g);
}
