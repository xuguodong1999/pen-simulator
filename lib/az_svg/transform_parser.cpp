#include "az/svg/transform_parser.h"
#include "./svgpp/transform_parser/transform.h"

using namespace az;
using namespace az::svg;

TransformMatrix2 az::svg::parse_transform(std::string_view in) {
    return az::svg::impl::svgpp::parse_transform(in);
}
