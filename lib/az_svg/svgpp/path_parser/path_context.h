#pragma once

#include "path_op.h"
#include "az/core/utils.h"

#include <svgpp/definitions.hpp>

#include <memory>

namespace az::svg::impl::svgpp {
    struct PathContext {
        PointerVec<PathOp> ops;

        void on_enter_element(::svgpp::tag::element::any);

        void on_exit_element();

        void path_move_to(double x, double y, ::svgpp::tag::coordinate::absolute);

        void path_line_to(double x, double y, ::svgpp::tag::coordinate::absolute);

        void path_cubic_bezier_to(
                double x1, double y1,
                double x2, double y2,
                double x, double y,
                ::svgpp::tag::coordinate::absolute);

        void path_quadratic_bezier_to(
                double x1, double y1,
                double x, double y,
                ::svgpp::tag::coordinate::absolute);

        void path_elliptical_arc_to(
                double rx, double ry, double x_axis_rotation,
                bool large_arc_flag, bool sweep_flag,
                double x, double y,
                ::svgpp::tag::coordinate::absolute);

        void path_close_subpath();

        void path_exit();
    };
}