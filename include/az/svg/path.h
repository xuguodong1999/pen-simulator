#pragma once

#include "az/svg/config.h"
#include "az/core/utils.h"
#include "az/core/geometry_def.h"

#include <memory>

class SkPath;

namespace az::svg::impl::svgpp {
    struct PathOp;
}

namespace az::svg {
    class AZSVG_EXPORT Path {
        std::shared_ptr<SkPath> sk_path;
        std::string d_path;
    public:
//        PointerVec<impl::svgpp::PathOp> svgpp_path;

        Path() = default;

        explicit Path(std::string_view in);

        Path(const Path &);

        Path &operator=(const Path &);

        Box2 bbox();

        Box2 bbox(const TransformMatrix2 &mat) const;

        SkPath *get_path() const;

        const std::string &get_d_path() const;
    };
}