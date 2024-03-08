#pragma once

#include "az/pen/config.h"
#include "az/pen/pen_op.h"

#include "az/core/geometry_def.h"
#include "az/core/utils.h"

namespace az::pen {
    class AZPEN_EXPORT PenFont final : public PenOp {
        UCharType text;
        Box2 box;
    public:
        explicit PenFont(UCharType text, const Box2 &box, const TransformMatrix2 &transform);

        Box2 bbox(const TransformMatrix2 &parent_trans) override;

        void on_paint(const TransformMatrix2 &parent_trans) final;
    };
}
