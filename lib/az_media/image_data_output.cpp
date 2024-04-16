#include "az/media/skia_utils.h"

#include "az/media/data_output.h"
#include "az/pen/pen_context.h"

#include <SkStream.h>
#include <SkPaint.h>
#include <SkCanvas.h>
#include <SkBitmap.h>
#include <SkPngEncoder.h>


#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;
using namespace az::media;
using namespace az::media::skia;

std::vector<uint8_t> az::media::pen_op_to_image(
        az::pen::PenOp *data,
        const PenOpPaintParam &param
) {
    SkPaint copy_paint = create_paint(param.bg_color, param.stroke_width);
    auto copy_bitmap = create_bitmap(param.frame_width, param.frame_height);
    SkCanvas canvas(copy_bitmap);
    std::vector<uint8_t> buffer;
    render_with_skia(data, param, [&](
            const SkBitmap &bitmap, std::string_view action
    ) {
        if ("bitmap" != action) { return; }
        skia_resize(canvas, bitmap, param);
        SkDynamicMemoryWStream out;
        SkPngEncoder::Options opt;
        opt.fZLibLevel = 9;
        bool res = SkPngEncoder::Encode(&out, copy_bitmap.pixmap(), opt);
        if (res) {
            buffer.reserve(out.bytesWritten());
            buffer.resize(out.bytesWritten());
            out.copyToAndReset(buffer.data());
        } else {
            SPDLOG_ERROR("SkPngEncoder::Encode:{}", res);
        }
    });
    return buffer;
}
