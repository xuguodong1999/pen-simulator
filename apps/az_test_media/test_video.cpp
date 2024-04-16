#include "az/media/data_output.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/pen/pen_graph.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;
using namespace az::data;
using namespace az::media;

TEST(test_media, video_gen) {
    const ShapeProvider printed_shape_provider = [&](const UCharType &label) {
        return nullptr;
    };
    auto pen_op = SynthesisTexGenerator::generate_next(
            "1+1=2",
            printed_shape_provider
    );
    ScalarType width = 64, height = 64;
    static const size_t MAX_RENDER_SIZE = 8192;
    static const az::ScalarType IDEAL_ITEM_SIZE = 88;
    static const az::ScalarType STROKE_WIDTH = 3.;
    static const az::ScalarType BLANK_PADDING_RATIO = 1.2;
    if (auto pen_graph = std::dynamic_pointer_cast<PenGraph>(pen_op)) {
        auto size = pen_graph->adjust_size_for_rendering(
                IDEAL_ITEM_SIZE, MAX_RENDER_SIZE / BLANK_PADDING_RATIO, BLANK_PADDING_RATIO);
        width = size.x();
        height = size.y();
    }
    if (!((0 < width && width < MAX_RENDER_SIZE + 1) && (0 < height && height < MAX_RENDER_SIZE + 1))) {
        SPDLOG_ERROR("skip invalid render size={}x{}", width, height);
        width = 64;
        height = 64;
    }
    int frame_width = 512;
    int frame_height = 512;
    int render_width = std::ceil(width);
    int render_height = std::ceil(height);
    if (render_width < frame_width && render_height < frame_height) {
        render_width = frame_width;
        render_height = frame_height;
        pen_op->fit_into_keep_ratio(render_width / BLANK_PADDING_RATIO, render_height / BLANK_PADDING_RATIO);
        pen_op->move_center_to(Vec2{render_width, render_height} / 2);
    }
    pen_op_to_video(pen_op.get(), PenOpPaintParam{
            .render_width=render_width,
            .render_height=render_height,
            .stroke_width=STROKE_WIDTH,
            .frame_width=frame_width < 0 ? render_width : frame_width,
            .frame_height=frame_height < 0 ? render_height : frame_height,
            .stroke_color=0xFFFFFFFF,
            .bg_color=0xFF000000,
    });
}
