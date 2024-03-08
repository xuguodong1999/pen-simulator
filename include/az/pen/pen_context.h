#pragma once

#include "az/pen/config.h"
#include "az/core/config.h"
#include "az/core/geometry_def.h"

#include <optional>
#include <stack>

class SkPath;

namespace az::svg {
    class Path;
}
namespace az::pen {
    class AZPEN_EXPORT PenContext final {
        using fn_draw_line_type = std::function<void(
                const ScalarType &x0, const ScalarType &y0,
                const ScalarType &x1, const ScalarType &y1
        )>;
        using fn_on_hierarchy_type = std::function<void(const std::vector<LabelType> &, const LabelType &)>;
        using fn_on_label_type = std::function<void(const LabelType &, const LabelType &)>;
        using fn_draw_skpath_type = std::function<void(const SkPath *, const TransformMatrix2 &)>;
        using fn_draw_path_type = std::function<void(std::string_view, const TransformMatrix2 &)>;
        // for font rotation case, we always need the origin box before transformation applied
        using fn_draw_text_type = std::function<void(const UCharType &, const Box2 &, const TransformMatrix2 &)>;
        fn_draw_line_type fn_draw_line;
        fn_draw_path_type fn_draw_path;
        fn_draw_skpath_type fn_draw_skpath;
        fn_draw_text_type fn_draw_text;
        fn_on_label_type fn_on_label;
        fn_on_hierarchy_type fn_on_hierarchy;
    public:
        void register_draw_line(const fn_draw_line_type &fn);

        void register_draw_path(const fn_draw_path_type &fn);

        void register_draw_skpath(const fn_draw_skpath_type &fn);

        void register_draw_text(const fn_draw_text_type &fn);

        void register_on_label(const fn_on_label_type &fn);

        void register_on_hierarchy(const fn_on_hierarchy_type &fn);

        void on_label(const LabelType &origin, const LabelType &fallback);

        void on_hierarchy(const std::vector<LabelType> &hierarchy, const LabelType &label);

        void draw_line(
                const ScalarType &x0, const ScalarType &y0,
                const ScalarType &x1, const ScalarType &y1
        );

        void draw_path(const az::svg::Path *, const TransformMatrix2 &);

        void draw_text(const UCharType &, const Box2 &, const TransformMatrix2 &);
    };
}