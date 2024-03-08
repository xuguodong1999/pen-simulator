#pragma once

#include "az/pen/config.h"
#include "az/pen/pen_op.h"

#include "az/core/geometry_def.h"
#include "az/core/utils.h"

namespace az::svg {
    class Path;
}
namespace az::pen {
    class AZPEN_EXPORT PenPath final : public PenOp {
        std::shared_ptr<az::svg::Path> path;
    public:
        PenPath() = default;

        explicit PenPath(std::string_view d);

        explicit PenPath(az::svg::Path *path, const TransformMatrix2 &trans);

        PenPath(const PenPath &);

        PenPath &operator=(const PenPath &);

        Box2 bbox(const TransformMatrix2 &parent_trans) override;

        void on_paint(const TransformMatrix2 &parent_trans) final;
    };
}
