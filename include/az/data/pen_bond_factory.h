#pragma once

#include "az/pen/pen_graph.h"
#include "az/data/config.h"

namespace az::data {
    struct AZDATA_EXPORT PenBondFactory {
        using Output = std::shared_ptr<az::pen::PenGraph>;

        static Output make_line_bond(
                const ShapeProvider &shape_provider,
                const az::Vec2 &from,
                const az::Vec2 &to,
                int8_t order = 1
        );

        static Output make_solid_wedge_bond(
                const ShapeProvider &shape_provider,
                const az::Vec2 &from,
                const az::Vec2 &to
        );

        static Output make_dash_wedge_bond(
                const ShapeProvider &shape_provider,
                const az::Vec2 &from,
                const az::Vec2 &to
        );

        static Output make_wave_bond(
                const ShapeProvider &shape_provider,
                const az::Vec2 &from,
                const az::Vec2 &to);

        static Output make_circle_bond(
                const ShapeProvider &shape_provider,
                const az::Box2 &bounding_box
        );
    };
}