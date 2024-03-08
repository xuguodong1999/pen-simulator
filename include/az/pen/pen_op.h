#pragma once

#include "az/pen/config.h"
#include "az/core/config.h"
#include "az/core/geometry_def.h"

namespace az::pen {
    class PenContext;

    class AZPEN_EXPORT PenOp {
    protected:
        TransformMatrix2 trans = TransformMatrix2::Identity();
        Box2 bounding_box;
        std::shared_ptr<PenContext> context;

        void invalidate_box();

    public:
        void set_context(std::shared_ptr<PenContext> c);

        virtual void add(const Vec2 &offset);

        virtual void rotate(ScalarType radians, const Vec2 &center);

        virtual void dot(ScalarType kx, ScalarType ky);

        virtual Box2 bbox(const TransformMatrix2 &parent_trans) = 0;

        virtual void on_paint(const TransformMatrix2 &parent_trans) = 0;

        void apply_transform(const TransformMatrix2 &matrix);

        const TransformMatrix2 &get_transform() const;

        void dot(ScalarType k);

        void move_tl_to(const Vec2 &dst);

        void move_center_to(const Vec2 &dst);

        void fit_into(ScalarType w, ScalarType h);

        void fit_into_keep_ratio(ScalarType w, ScalarType h);

        void rotate(ScalarType radians);

        void rotate_deg(ScalarType degree);

        Box2 bbox();

        void on_paint();

        /**
         * @deprecated
         */
        virtual Vec2Array as_points();
    };
}