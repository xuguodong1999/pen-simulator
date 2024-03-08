#pragma once

#include "az/core/geometry_def.h"

#include <boost/array.hpp>

namespace az::svg::impl::svgpp {
    struct TransformEventsPolicy {
        using context_type = TransformMatrix2;

        static void transform_matrix(TransformMatrix2 &transform, const boost::array<double, 6> &matrix);

        static void transform_translate(TransformMatrix2 &transform, double tx, double ty);

        static void transform_translate(TransformMatrix2 &transform, double tx);

        static void transform_scale(TransformMatrix2 &transform, double sx, double sy);

        static void transform_scale(TransformMatrix2 &transform, double scale);

        static void transform_rotate(TransformMatrix2 &transform, double angle);

        static void transform_rotate(TransformMatrix2 &transform, double angle, double cx, double cy);

        static void transform_skew_x(TransformMatrix2 &transform, double angle);

        static void transform_skew_y(TransformMatrix2 &transform, double angle);
    };
}