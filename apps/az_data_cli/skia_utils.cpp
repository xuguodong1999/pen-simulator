#include "skia_utils.h"
#include "az/core/utils.h"
#include "az/core/encoding.h"
#include "az/pen/pen_context.h"
#include "az/js/qjs_wrapper.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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
#define RENDER_FRAMES

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

const SkColor BACKGROUND_COLOR = SK_ColorBLACK;
const SkColor SCRIPT_COLOR = SkColorSetARGB(0x8F, 0xFF, 0xFF, 0xFF);

static void render_with_skia(
        az::pen::PenOp *data,
        const int &w,
        const int &h,
        const float &stroke_width,
        const fn_on_frame_type &on_frame
) {
    SkPaint line_paint = create_paint(SCRIPT_COLOR, stroke_width);
    auto bitmap = create_bitmap(w, h);
    SkCanvas canvas(bitmap);
    canvas.drawColor(BACKGROUND_COLOR);

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

static void resize_keep_ratio(
        cv::Mat &dst,
        const cv::Mat &src,
        const cv::Size &dst_size
) {
    // https://stackoverflow.com/questions/28562401/resize-an-image-to-a-square-but-keep-aspect-ratio-c-opencv
    double h1 = dst_size.width * (src.rows / (double) src.cols);
    double w2 = dst_size.height * (src.cols / (double) src.rows);
    cv::Scalar pad_color = src.at<cv::Vec4d>(0, 0);
    if (h1 <= dst_size.height) {
        cv::resize(src, dst, cv::Size(dst_size.width, std::ceil(h1)), 0, 0, cv::INTER_NEAREST);
    } else {
        cv::resize(src, dst, cv::Size(std::ceil(w2), dst_size.height), 0, 0, cv::INTER_NEAREST);
    }
    int top = (dst_size.height - dst.rows) / 2;
    int down = (dst_size.height - dst.rows + 1) / 2;
    int left = (dst_size.width - dst.cols) / 2;
    int right = (dst_size.width - dst.cols + 1) / 2;
    cv::copyMakeBorder(dst, dst, top, down, left, right, cv::BORDER_CONSTANT, pad_color);
}

void display_in_opencv_highgui(
        az::pen::PenOp *data,
        const int &w,
        const int &h,
        const float &stroke_width,
        std::string_view win_name,
        const int &win_width,
        const int &win_height
) {
    static const auto win_id = "qwertyuiop";
    const auto render_looper = [&](bool frame_mode = false) {
        render_with_skia(data, w, h, stroke_width, [&](const SkBitmap &bitmap, std::string_view action) {
#ifndef RENDER_FRAMES
            if (action != "bitmap") { return; }
#endif
#ifdef USE_ARGB
            cv::Mat frame(cv::Size{bitmap.width(), bitmap.height()}, CV_8UC4, bitmap.getPixels());
#else
            cv::Mat frame(cv::Size{bitmap.width(), bitmap.height()}, CV_8UC1, bitmap.getPixels());
#endif
            resize_keep_ratio(frame, frame, cv::Size{win_width, win_height});
#ifdef USE_ARGB
            cv::cvtColor(frame, frame, cv::COLOR_RGBA2BGRA);
#endif
            cv::imshow(win_id, frame);
            cv::setWindowTitle(win_id, win_name.data());
            if (frame_mode) {
                if ("path" == action) {
                    cv::waitKey(25);
                } else {
                    // skip frames
//                    if (random_float(0., 1.) < 0) {
//                    if (random_float(0., 1.) < 1) {
                    if (random_float(0., 1.) < 0.2) {
                        cv::waitKey(1);
                    }
                }
            }
        });
        cv::setWindowTitle(win_id, "Press SPACE to repeat, press other key to continue");
    };
#ifdef RENDER_FRAMES
    render_looper(true);
    while (true) {
//        if (cv::waitKey(2000) == ' ') {
        if (cv::waitKey(0) == ' ') {
            render_looper(true);
        } else {
            break;
        }
    }
#else
    // fast check correctness manually
    render_looper(false);
    cv::waitKey(2000);
#endif
}

void save_frames(
        az::pen::PenOp *data,
        const int &w,
        const int &h,
        const float &stroke_width,
        std::string_view dst_directory,
        std::string_view image_prefix,
        const int &image_width,
        const int &image_height
) {
    int64_t frame_idx = 0;
    fs::path out_dir;
    try {
        out_dir = fs::canonical(dst_directory);
    } catch (std::exception &e) {
        SPDLOG_ERROR("save_frames got exception - {}", e.what());
        return;
    }
    std::string path = out_dir.string();
    if (!path.ends_with(fs::path::preferred_separator)) {
        path += fs::path::preferred_separator;
    }
    SPDLOG_INFO("output directory={}", path);

    SkPaint copy_paint = create_paint(SCRIPT_COLOR, stroke_width);
    auto copy_bitmap = create_bitmap(image_width, image_height);
    SkCanvas canvas(copy_bitmap);

    render_with_skia(data, w, h, stroke_width, [&](const SkBitmap &bitmap, std::string_view action) {
        canvas.drawColor(BACKGROUND_COLOR);

        auto kx = static_cast<SkScalar>(image_width) / bitmap.width();
        auto ky = static_cast<SkScalar>(image_height) / bitmap.height();
        auto k = static_cast<SkScalar>((std::min)(kx, ky));

        if (kx < ky) {
            auto dy = static_cast<SkScalar>(image_height - bitmap.height() * k) / 2;
            canvas.translate(0, dy);
        } else {
            auto dx = static_cast<SkScalar>(image_width - bitmap.width() * k) / 2;
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
        std::string file_name = fmt::format("{}{}_{:08d}.png", path, image_prefix, frame_idx);
        SPDLOG_INFO("save to: {}", file_name);
        SkFILEWStream out(file_name.c_str());
        SkPngEncoder::Options opt;
        opt.fZLibLevel = 9;
        bool res = SkPngEncoder::Encode(&out, copy_bitmap.pixmap(), opt);
        if (!res) {
            SPDLOG_INFO("SkPngEncoder::Encode:{}", res);
        } else {
            frame_idx++;
        }
    });
}
