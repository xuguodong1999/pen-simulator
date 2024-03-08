#pragma once

#include "az/pen/config.h"
#include "az/pen/pen_char.h"
#include "az/core/utils.h"

namespace az::pen {
    class AZPEN_EXPORT PenString final : public PenOp {
    public:
        std::vector<PenChar> data;

        Box2 bbox(const TransformMatrix2 &parent_trans) override;

        void on_paint(const TransformMatrix2 &parent_trans) final;
    };
};
