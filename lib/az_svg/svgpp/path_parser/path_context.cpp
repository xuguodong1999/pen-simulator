#include "path_context.h"

using namespace ::svgpp;
using namespace az::svg::impl::svgpp;

void PathContext::on_enter_element(tag::element::any) {
    ops.push_back(std::make_shared<PathEnterElementOp>());
}

void PathContext::on_exit_element() {
    ops.push_back(std::make_shared<PathExitElementOp>());
}

void PathContext::path_move_to(double x, double y, tag::coordinate::absolute) {
    ops.push_back(std::make_shared<PathMoveToOp>(x, y));
}

void PathContext::path_line_to(double x, double y, tag::coordinate::absolute) {
    ops.push_back(std::make_shared<PathLineToOp>(x, y));
}

void PathContext::path_cubic_bezier_to(
        double x1, double y1,
        double x2, double y2,
        double x, double y,
        tag::coordinate::absolute) {
    ops.push_back(std::make_shared<PathCubicBezierToOp>(x1, y1, x2, y2, x, y));
}

void PathContext::path_quadratic_bezier_to(
        double x1, double y1,
        double x, double y,
        tag::coordinate::absolute) {
    ops.push_back(std::make_shared<PathQuadraticBezierToOp>(x1, y1, x, y));
}

void PathContext::path_elliptical_arc_to(
        double rx, double ry, double x_axis_rotation,
        bool large_arc_flag, bool sweep_flag,
        double x, double y,
        tag::coordinate::absolute) {
    ops.push_back(std::make_shared<PathEllipticalArcToOp>(
            rx, ry, x_axis_rotation, large_arc_flag, sweep_flag, x, y));
}

void PathContext::path_close_subpath() {
    ops.push_back(std::make_shared<PathCloseSubOp>());
}

void PathContext::path_exit() {
    ops.push_back(std::make_shared<PathExitOp>());
}