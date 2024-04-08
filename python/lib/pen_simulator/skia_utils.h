#pragma once

#include "az/pen/pen_op.h"

#include <string>

struct PenOpPaintParam {
    int render_width;
    int render_height;
    float stroke_width;
    int frame_width;
    int frame_height;
    uint32_t stroke_color;
    uint32_t bg_color;
};

std::vector<uint8_t> pen_op_to_png(
        az::pen::PenOp *data,
        const PenOpPaintParam &param
);

std::vector<uint8_t> pen_op_to_mp4(
        az::pen::PenOp *data,
        const PenOpPaintParam &param
);