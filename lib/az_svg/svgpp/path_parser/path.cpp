#include "path.h"

#include "path_context.h"

#include <svgpp/svgpp.hpp>

using namespace svgpp;
using namespace az::svg::impl::svgpp;

namespace az::svg::impl::svgpp {
    PointerVec<PathOp> svgpp_parse_path(std::string_view in) {
        PathContext context;
        value_parser<
                tag::type::path_data,
                path_policy<policy::path::no_shorthands>
        >::parse(tag::attribute::d(), context, in, tag::source::attribute());
        return std::move(context.ops);
    }
}