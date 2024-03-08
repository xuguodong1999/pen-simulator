#include "graph_impl.h"
#include "mathjax_json_io.h"

#include <iostream>

using namespace az::tex;

namespace az::tex::mathjax::impl {
    void convert_json_output_to_graph(const Output &json_graph, GraphImpl &graph);
}

mathjax::Graph az::tex::convert_tex_to_graph(std::string_view in) {
    auto json_str = az::tex::convert_tex_to_json(in);
    auto json = az::tex::mathjax::impl::Output::from_json(json_str);
    mathjax::Graph g;
    mathjax::impl::convert_json_output_to_graph(json, *(g.get_pimpl()));
    return g;
}
