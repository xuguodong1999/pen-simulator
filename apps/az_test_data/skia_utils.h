#pragma once

#include "az/pen/pen_op.h"

#include <string>

void draw_with_skia(
        az::pen::PenOp *data,
        const std::string &name,
        const int &w,
        const int &h,
        const float &stroke_width,
        const std::string_view &msg = "",
        bool display_mode = true
);


