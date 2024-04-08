#include "skia_utils.h"
#include "az/core/utils.h"
#include "az/core/encoding.h"
#include "az/pen/pen_context.h"
#include "az/js/qjs_wrapper.h"

#include <SkPoint3.h>
#include <SkPath.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkTextBlob.h>
#include <SkFontMgr.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImage.h>
#include <SkPixelRef.h>
#include <SkPngEncoder.h>

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;

//#define USE_ARGB

static SkBitmap create_bitmap(const int &w, const int &h) {
    SkBitmap bitmap;
    bitmap.setInfo(SkImageInfo::Make(
            w,
            h,
#ifdef USE_ARGB
            SkColorType::kRGBA_8888_SkColorType,
#else
            SkColorType::kGray_8_SkColorType,
#endif
            SkAlphaType::kPremul_SkAlphaType));
    bitmap.allocPixels();
    return bitmap;
}

static SkMatrix convert_eigen_33_to_sk_matrix(const TransformMatrix2 &mat) {
    return SkMatrix::MakeAll(
            mat(0, 0), mat(0, 1), mat(0, 2),
            mat(1, 0), mat(1, 1), mat(1, 2),
            mat(2, 0), mat(2, 1), mat(2, 2));
}

static SkPaint create_paint(const SkColor &color, const float &stroke_width = 12) {
    SkPaint paint;
    paint.setStrokeWidth(stroke_width);
    paint.setColor(color);
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::Join::kRound_Join);
    return paint;
}

static SkFont create_font(const UCharType &label = "😅") {
    SkFont font;
    auto label_buf = az::js::QjsWrapper::convert_unicode_str_to_buffer(label);
    auto tf = SkFontMgr::RefDefault()->matchFamilyStyleCharacter(
            "Noto Color Emoji", SkFontStyle{}, nullptr, 0, SkUnichar{label_buf});
    if (!tf) {
        tf = SkFontMgr::RefDefault()->matchFamilyStyleCharacter(
                nullptr, SkFontStyle{}, nullptr, 0, SkUnichar{label_buf});
    }
    if (tf) {
        font.setTypeface(tf);
    } else {
        SPDLOG_ERROR("failed to find typeface");
    }
    font.setSubpixel(true);
    return font;
}

using fn_on_frame_type = std::function<void(const SkBitmap &, std::string_view)>;

static void render_with_skia(
        az::pen::PenOp *data,
        const PenOpPaintParam &param,
        const fn_on_frame_type &on_frame
) {
    SkPaint line_paint = create_paint(param.stroke_color, param.stroke_width);
    auto bitmap = create_bitmap(param.render_width, param.render_height);
    SkCanvas canvas(bitmap);
    canvas.drawColor(param.bg_color);

    auto pen_context = std::make_shared<PenContext>();
    pen_context->register_draw_line([&](
            const ScalarType &x0, const ScalarType &y0,
            const ScalarType &x1, const ScalarType &y1
    ) {
        canvas.drawLine(SkPoint{x0, y0}, SkPoint{x1, y1}, line_paint);
        on_frame(bitmap, "line");
    });
    pen_context->register_draw_skpath([&](
            const SkPath *path,
            const TransformMatrix2 &mat
    ) {
        SkMatrix sk_mat = convert_eigen_33_to_sk_matrix(mat);
        canvas.setMatrix(sk_mat);
        canvas.drawPath(*path, line_paint);
        canvas.resetMatrix();
        on_frame(bitmap, "path");
    });
    pen_context->register_draw_text([&](
            const UCharType &label,
            const Box2 &box,
            const TransformMatrix2 &mat
    ) {
        auto font = create_font(label);
        auto font_size = (box.max() - box.min()).maxCoeff();
        font.setSize(font_size);
        SkRect real_box;
        font.measureText(label.c_str(), label.length(), SkTextEncoding::kUTF8, &real_box, &line_paint);
        SkMatrix sk_mat = convert_eigen_33_to_sk_matrix(mat);
        canvas.setMatrix(sk_mat);
        canvas.drawString(label.c_str(), 0, 0, font, line_paint);
        canvas.resetMatrix();
        on_frame(bitmap, "text");
    });

    data->set_context(pen_context);
    data->on_paint();
    on_frame(bitmap, "bitmap");
    data->set_context(nullptr);
}

static void skia_resize(SkCanvas &canvas, const SkBitmap &bitmap, const PenOpPaintParam &param) {
    canvas.drawColor(param.bg_color);

    auto kx = static_cast<SkScalar>(param.frame_width) / bitmap.width();
    auto ky = static_cast<SkScalar>(param.frame_height) / bitmap.height();
    auto k = static_cast<SkScalar>((std::min)(kx, ky));

    if (kx < ky) {
        auto dy = static_cast<SkScalar>(param.frame_height - bitmap.height() * k) / 2;
        canvas.translate(0, dy);
    } else {
        auto dx = static_cast<SkScalar>(param.frame_width - bitmap.width() * k) / 2;
        canvas.translate(dx, 0);
    }
    canvas.scale(k, k);

//        canvas.writePixels(bitmap, 0, 0); // scale not work, use image instead
//        auto image = SkImages::RasterFromBitmap(bitmap); // always mutable, buffer copy leads to low performance
    auto image = SkImages::RasterFromData(
            SkImageInfo::Make(
                    bitmap.width(), bitmap.height(),
                    bitmap.colorType(), bitmap.alphaType()
            ),
            SkData::MakeWithoutCopy(
                    bitmap.getPixels(), bitmap.computeByteSize()
            ),
            bitmap.rowBytes()
    ); // comments said the pixels data will *not* be copied, just trust it.
    canvas.drawImage(image, 0, 0);
    canvas.resetMatrix();
}

std::vector<uint8_t> pen_op_to_png(
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

std::vector<uint8_t> pen_op_to_mp4(
        az::pen::PenOp *data,
        const PenOpPaintParam &param
) {
    SkPaint copy_paint = create_paint(param.stroke_color, param.stroke_width);
    auto copy_bitmap = create_bitmap(param.frame_width, param.frame_height);
    SkCanvas canvas(copy_bitmap);
    std::vector<uint8_t> buffer;
    render_with_skia(data, param, [&](
            const SkBitmap &bitmap, std::string_view action
    ) {
        skia_resize(canvas, bitmap, param);
        SkDynamicMemoryWStream out;
        SkPngEncoder::Options opt;
        opt.fZLibLevel = 9;
        bool res = SkPngEncoder::Encode(&out, copy_bitmap.pixmap(), opt);
        if (res) {
            // TODO: encode buffer into mp4
            buffer.reserve(out.bytesWritten());
            buffer.resize(out.bytesWritten());
            out.copyToAndReset(buffer.data());
        } else {
            SPDLOG_ERROR("SkPngEncoder::Encode:{}", res);
        }
    });
    return buffer;
}
