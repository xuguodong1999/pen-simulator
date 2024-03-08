#include "skia_utils.h"
#include "az/pen/pen_op.h"
#include "az/pen/pen_context.h"
#include "az/core/utils.h"
#include "az/core/encoding.h"
#include "az/js/qjs_wrapper.h"

#include <SkPoint3.h>
#include <SkPath.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkTextBlob.h>
#include <SkFontMgr.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkPngEncoder.h>

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;

static SkBitmap create_bitmap(const int &w, const int &h) {
    SkBitmap bitmap;
    bitmap.setInfo(SkImageInfo::Make(
            w,
            h,
            SkColorType::kRGBA_8888_SkColorType,
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

static SkPaint create_paint(SkColor color, const float &stroke_width = 12) {
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

//#define DRAW_MASK
const float MSG_FONT_BORDER = 50;
const float MSG_FONT_SIZE = 15;

void draw_with_skia(
        az::pen::PenOp *data,
        const std::string &name,
        const int &w,
        const int &h,
        const float &stroke_width,
        const std::string_view &msg,
        bool display_mode
) {
    SkPaint line_paint = create_paint(SkColorSetARGB(0x8F, 0xFF, 0xFF, 0xFF), stroke_width);
    SkPaint mask_paint = create_paint(SkColorSetARGB(0x1F, 0xFF, 0x00, 0x00), stroke_width);
    const float msg_font_border = msg.empty() ? 0 : MSG_FONT_BORDER;
    auto bitmap = create_bitmap(w, h + msg_font_border);
    SkCanvas canvas(bitmap);
    canvas.drawColor(SK_ColorBLACK);
    auto pen_context = std::make_shared<PenContext>();
    static size_t frame_index = 0;
    const auto on_frame = [&](bool is_svg) {
        SkFILEWStream out(get_tmp_root(fmt::format("{}_{:06d}.png", name, frame_index++)).c_str());
        SkPngEncoder::Options opt;
        opt.fZLibLevel = 9;
        bool res = SkPngEncoder::Encode(&out, bitmap.pixmap(), opt);
        SPDLOG_INFO("SkPngEncoder::Encode:{}", res);
    };
    pen_context->register_draw_line([&](
            const ScalarType &x0, const ScalarType &y0,
            const ScalarType &x1, const ScalarType &y1
    ) {
        canvas.drawLine(SkPoint{x0, y0}, SkPoint{x1, y1}, line_paint);
        on_frame(false);
    });
    pen_context->register_draw_skpath([&](
            const SkPath *path,
            const TransformMatrix2 &mat
    ) {
        SkMatrix sk_mat = convert_eigen_33_to_sk_matrix(mat);
        SkPath transformed = path->makeTransform(sk_mat);
        canvas.drawPath(transformed, line_paint);
#ifdef DRAW_MASK
        canvas.drawRect(transformed.computeTightBounds(), mask_paint);
#endif
        on_frame(true);
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
#ifdef DRAW_MASK
        canvas.drawRect(real_box, mask_paint);
#endif
        canvas.drawString(label.c_str(), 0, 0, font, line_paint);
        canvas.resetMatrix();
    });
    data->set_context(pen_context);
    data->on_paint();

    data->set_context(nullptr);

    if (display_mode && !msg.empty()) {
        SkPaint msg_paint = create_paint(SK_ColorRED, 1);
        SkFont font;// = create_font("好");
        font.setSize(MSG_FONT_SIZE);
        canvas.drawString(msg.data(), MSG_FONT_SIZE, bitmap.height() - msg_font_border, font, msg_paint);
    }

    SkDynamicMemoryWStream out;
//    SkFILEWStream out(get_tmp_root(name).c_str());
    SkPngEncoder::Options opt;
    opt.fZLibLevel = 9;
    bool res = SkPngEncoder::Encode(&out, bitmap.pixmap(), opt);
    SPDLOG_INFO("SkPngEncoder::Encode:{}", res);
}
