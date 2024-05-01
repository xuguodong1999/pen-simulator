#include "az/tex/api.h"
#include "az/core/encoding.h"
#include "az/core/utils.h"
#include "az/media/skia_utils.h"

#include <gtest/gtest.h>

#include <SkPoint3.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkFontMgr.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkPngEncoder.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <stack>
#include <fstream>

using namespace az;
using az::tex::mathjax::NodeProps;
using az::tex::mathjax::NodePropsPtrHasher;

template<>
struct fmt::formatter<Box2> {
    static constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const Box2 &bbox, FormatContext &ctx) -> decltype(ctx.out()) {
        const auto &tl = bbox.min();
        const auto &br = bbox.max();
        std::ostringstream osm;
        osm << "(" << tl.x() << "," << tl.y() << "),(" << br.x() << "," << br.y() << ")";
        return fmt::format_to(ctx.out(), "{}", osm.str());
    }
};

TEST(test_tex, parse_mathjax_svg_json_graph) {
    const char *tex_str = R"xxxxx(
\begin{pmatrix}
 你好 & c_1 & c_1^2 & \cdots & c_1^n \\
 😅 & c_2 & c_2^2 & \cdots & c_2^n \\
 \vdots  & \vdots& \vdots & \ddots & \vdots \\
 1 & c_m & c_m^2 & \cdots & c_m^n \\
 \sum_{n=1}^{\infty} \frac{1}{3^n \cdot n} & \sum_{n=1}^{\infty} \frac{1}{4^n \cdot n} & 😅 & 😅 & 😅 \\
 \frac{2}{5} \sum_{k=0}^{\infty} \frac{1}{25^k(2 k+1)} & \frac{2}{7} \sum_{k=0}^{\infty} \frac{1}{49^k(2 k+1)} & 😅 & 😅 & 😅 \\
 7 \sum_{n=1}^{\infty} \frac{1}{16^n \cdot n} & 5 \sum_{n=1}^{\infty} \frac{1}{25^n \cdot n} &  \sum_{n=1}^{\infty} \frac{1}{81^n \cdot n} & 😅 & 😅
\end{pmatrix}
)xxxxx";
    SPDLOG_INFO("tex={}", tex_str);
    auto graph = az::tex::convert_tex_to_graph(tex_str);

    std::stack<TransformMatrix2> transform_stack;
    {
        TransformMatrix2 identity;
        identity.setIdentity();
        transform_stack.emplace(std::move(identity));
    }
    std::vector<Box2> coord_stack;
    std::unordered_map<const NodeProps *, Box2, NodePropsPtrHasher> id_box_map;
    auto on_discover_node = [&](const NodeProps &node) {
        // on_discover_node
        auto &prev_transform = transform_stack.top();
        transform_stack.emplace(transform_stack.top() * node.transform);
        auto &current_transform = transform_stack.top();
        SPDLOG_INFO("Discover vertex: {}, current transform=[\n{}\n]",
                    node.id, current_transform);
        if ("svg" == node.svg_type && node.width > 0 && node.height > 0) {
            coord_stack.emplace_back(
                    Vec2{node.x, node.y},
                    Vec2{node.x + node.width, node.y + node.height}
            );
        } else if ("text" == node.svg_type) {
            ScalarType offset = node.font_size * 0.92;
            Box2 orig_box(Vec2{0, -offset}, Vec2{offset, 0});
            Transform2 trans;
            trans.matrix() = current_transform;
            auto box = orig_box.transformed(trans);
            SPDLOG_INFO("Discover text: {}, box={}", node.id, box);
            id_box_map.insert(std::make_pair(&node, std::move(box)));
        } else if (!node.path_id.empty()) {
            auto box = graph.bbox(node, current_transform);
            if (!box.isEmpty()) {
                // skip root svg
                if (coord_stack.size() >= 2) {
                    auto coord_box = coord_stack.back();
                    auto tl = coord_box.min();
                    Transform2 trans;
                    trans.matrix() = prev_transform;
                    coord_box.transform(trans);

                    box = box.translated(-tl).clamp(coord_box);
                }
                SPDLOG_INFO("Discover path: {}, box={}", node.id, box);
                id_box_map.insert(std::make_pair(&node, std::move(box)));
            }
        }
    };
    auto on_finish_node = [&](const NodeProps &node) {
        // on_finish_node
        if ("svg" == node.svg_type && node.width > 0 && node.height > 0) {
            coord_stack.pop_back();
        }
        transform_stack.pop();
        const auto &current_transform = transform_stack.top();
        SPDLOG_INFO("Finish vertex: {}, transform stack size={}, current transform=[\n{}\n]",
                    node.id, transform_stack.size(), current_transform);
    };

    graph.traverse(on_discover_node, on_finish_node);

    Box2 tex_box;
    for (auto &[node_ptr, box]: id_box_map) {
        tex_box.extend(box);
    }
    SPDLOG_INFO("origin tex_box={}", tex_box);
    const auto &tl0 = tex_box.min();
    const auto &br0 = tex_box.max();
    float x_offset = tl0.x();
    float y_offset = tl0.y();
    ScalarType dst_width = 4096;
    ScalarType scale = dst_width / (std::abs)(br0.x() - tl0.x());
    ScalarType dst_height = scale * (std::abs)(br0.y() - tl0.y());
    SPDLOG_INFO("scale={},size=({}x{})", scale, dst_width, dst_height);

    const int WIDTH = std::ceil(dst_width);
    const int HEIGHT = std::ceil(dst_height);
    SkBitmap bitmap;
    bitmap.setInfo(SkImageInfo::Make(
            WIDTH,
            HEIGHT,
            SkColorType::kRGBA_8888_SkColorType,
            SkAlphaType::kPremul_SkAlphaType));
    bitmap.allocPixels();
    {
        SkCanvas canvas(bitmap);
        canvas.scale(scale, scale);
        canvas.translate(-x_offset, -y_offset);
        canvas.drawColor(SK_ColorGRAY);

        for (auto &[node_ptr, box]: id_box_map) {
            const auto &tl = box.min();
            const auto &br = box.max();
            SPDLOG_INFO("({},{},{},{})", tl.x(), tl.y(), br.x(), br.y());
            SkPaint paint;
            paint.setColor(SkColorSetARGB(0x5F, 0x00, 0x00, 0xFF));
            canvas.drawRect(SkRect::MakeLTRB(tl.x(), tl.y(), br.x(), br.y()), paint);
            if (!node_ptr->label.empty()) {
                std::string label_hex = fmt::format("0x{}", node_ptr->label);
                auto label_buf = convert_unicode_hex_to_buffer(label_hex);
                std::string label = convert_unicode_buffer_to_str(label_buf);
                SPDLOG_INFO("draw label {} as {}", label_hex, label);
                SkFont font;
                {
                    auto tf = az::media::skia::get_skia_font_manager()->matchFamilyStyleCharacter(
                            nullptr, SkFontStyle{}, nullptr, 0, SkUnichar{label_buf});
                    if (tf) {
                        font.setTypeface(tf);
                    } else {
                        SPDLOG_ERROR("failed to find typeface");
                    }
                    font.setSubpixel(true);
                    font.setSize(20 / scale);
                }
                SkPaint paint2;
                paint2.setColor(SK_ColorBLACK);
//                canvas.drawString(
//                        label.data(),
//                        tl.x(),
//                        tl.y() + 200,
//                        font,
//                        paint2
//                );
            }
        }
    }
    SkDynamicMemoryWStream out;
//    SkFILEWStream out(get_tmp_root("1.png").c_str());
    SkPngEncoder::Options opt;
    opt.fZLibLevel = 9;
    bool res = SkPngEncoder::Encode(&out, bitmap.pixmap(), opt);
    SPDLOG_INFO("SkPngEncoder::Encode:{}", res);
}
