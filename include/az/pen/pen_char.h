#pragma once

#include "az/pen/config.h"
#include "az/pen/pen_stroke.h"
#include "az/core/utils.h"

namespace az::pen {

    class AZPEN_EXPORT PenChar final : public PenOp {
    public:
        std::vector<PenStroke> data;

        Box2 bbox(const TransformMatrix2 &parent_trans) final;

        void on_paint(const TransformMatrix2 &parent_trans) final;

        Vec2Array as_points() final;
    };
};
