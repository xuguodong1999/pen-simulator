#pragma once

#include "az/media/config.h"
#include "az/pen/pen_op.h"

#include <string>

namespace az::media {
    struct AZMEDIA_EXPORT PenOpPaintParam {
        int render_width;
        int render_height;
        float stroke_width;
        int frame_width;
        int frame_height;
        uint32_t stroke_color;
        uint32_t bg_color;
    };

    std::vector<uint8_t> AZMEDIA_EXPORT pen_op_to_image(
            az::pen::PenOp *data,
            const PenOpPaintParam &param
    );

    std::vector<uint8_t> AZMEDIA_EXPORT pen_op_to_video(
            az::pen::PenOp *data,
            const PenOpPaintParam &param
    );
}
