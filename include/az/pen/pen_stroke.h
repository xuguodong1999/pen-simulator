#pragma once

#include "az/pen/config.h"
#include "az/pen/pen_op.h"

#include "az/core/geometry_def.h"
#include "az/core/utils.h"

namespace az::pen {
    class AZPEN_EXPORT PenStroke final : public PenOp {
        // data layout: [2,col]
        //      x1,x2,x3,...
        //      y1,y2,y3,...
        Mat2 points;
    public:
        void apply_add(const Vec2 &offset);

        void apply_rotate(ScalarType radians, const Vec2 &center);

        void apply_dot(ScalarType kx, ScalarType ky);

        Box2 bbox(const TransformMatrix2 &parent_trans) override;

        void set_data(SizeType size, const std::function<void(ScalarType &x, ScalarType &y)> &on_next);

        void on_paint(const TransformMatrix2 &parent_trans) final;

        Vec2Array as_points() final;

        size_t count() const;
    };
}
