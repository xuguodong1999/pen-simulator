#pragma once

#include "az/pen/config.h"
#include "az/pen/pen_char.h"
#include "az/pen/pen_item_info.h"
#include "az/core/utils.h"

namespace az::pen {
    /**
     * work as a composition of PenOp derivatives
     */
    struct AZPEN_EXPORT PenGraph final : public PenOp {
        PointerVec<PenItemInfo> extra;
        PointerVec<PenOp> data;

        PenGraph() = default;

        PenGraph(const PenGraph &);

        PenGraph &operator=(const PenGraph &);

        void rotate_skeleton(ScalarType radians, const Vec2 &center);

        Box2 bbox(const TransformMatrix2 &parent_trans) override;

        void on_paint(const TransformMatrix2 &parent_trans) final;

        ScalarType get_average_item_size();

        Vec2 adjust_size_for_rendering(
                const ScalarType &ideal_item_size,
                const ScalarType &max_render_size,
                const ScalarType &padding_ratio
        );
    };
};
