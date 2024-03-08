#pragma once

#include "az/core/geometry_def.h"

namespace az::svg::impl::svgpp {
    struct PathOp {
    };

    struct PathEnterElementOp : PathOp {
    };

    struct PathExitElementOp : PathOp {
    };

    struct PathMoveToOp : public PathOp {
        Vec2 point;

        explicit PathMoveToOp(double x, double y) : point{x, y} {}
    };

    struct PathLineToOp : PathOp {
        Vec2 point;

        explicit PathLineToOp(double x, double y) : point{x, y} {}
    };

    struct PathCubicBezierToOp : PathOp {
        Vec2 point1, point2, point;

        explicit PathCubicBezierToOp(double x1, double y1, double x2, double y2, double x, double y)
                : point1{x1, y1}, point2{x2, y2}, point{x, y} {}
    };

    struct PathQuadraticBezierToOp : PathOp {
        Vec2 point1, point;

        explicit PathQuadraticBezierToOp(double x1, double y1, double x, double y) : point1{x1, y1}, point{x, y} {}
    };

    struct PathEllipticalArcToOp : PathOp {
        double rx, ry, x_axis_rotation;
        bool large_arc_flag, sweep_flag;
        Vec2 point;

        explicit PathEllipticalArcToOp(
                double rx, double ry, double x_axis_rotation,
                bool large_arc_flag, bool sweep_flag,
                double x, double y) :
                rx(rx), ry(ry), x_axis_rotation(x_axis_rotation),
                large_arc_flag(large_arc_flag), sweep_flag(sweep_flag), point{x, y} {}
    };

    struct PathCloseSubOp : PathOp {
    };

    struct PathExitOp : PathOp {
    };
}