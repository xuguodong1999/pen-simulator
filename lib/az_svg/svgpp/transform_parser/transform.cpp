#include "transform.h"
#include "transform_events_policy.h"

#include <svgpp/svgpp.hpp>

using namespace az;

TransformMatrix2 az::svg::impl::svgpp::parse_transform(std::string_view in) {
    using namespace ::svgpp;
    TransformMatrix2 transform = TransformMatrix2::Identity();
    value_parser<
            tag::type::transform_list,
            transform_policy<policy::transform::raw>,
            transform_events_policy<TransformEventsPolicy>
    >::parse(tag::attribute::transform(), transform, in, tag::source::attribute());
    return transform;
}