#include "az/svg/path.h"
#include "az/svg/path_parser.h"

//#include "./svgpp/path_parser/path.h"

using namespace az::svg;

Path az::svg::parse_path(std::string_view in) {
    Path path(in);
//    PointerVec<impl::svgpp::PathOp> svgpp_path_value = impl::svgpp::svgpp_parse_path(in);
//    path.svgpp_path = std::move(svgpp_path_value);
    return path;
}
