#include "az/media/data_output.h"
#include "az/data/couch_reader.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/pen/pen_graph.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>

#include <fstream>

using namespace az;
using namespace az::pen;
using namespace az::data;
using namespace az::media;

TEST(test_media, video_gen) {
    CouchDatasetReader hw_reader;
    hw_reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
            "Couch_Digit_195",
//            "Couch_GB1_188",
//            "Couch_GB2_195",
            "Couch_Letter_195",
            "Couch_Symbol_130"
    });
    const ShapeProvider shape_provider = [&](const UCharType &label) {
        return hw_reader.select(label);
    };
    auto pen_op = SynthesisTexGenerator::generate_next(
            R"(\displaylines {
1+1=2 \\
1+1=2 \\
1+1=2 \\
1+1=2 \\
1+1=2 \\
1+1=2 \\
})",
            shape_provider
    );
    ScalarType width = 64, height = 64;
    static const size_t MAX_RENDER_SIZE = 8192;
    static const az::ScalarType IDEAL_ITEM_SIZE = 88;
    static const az::ScalarType STROKE_WIDTH = 5.;
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
    int frame_width = 3840 / 4;
    int frame_height = 2560 / 4;
    int render_width = std::ceil(width);
    int render_height = std::ceil(height);
    if (render_width < frame_width && render_height < frame_height) {
        render_width = frame_width;
        render_height = frame_height;
        pen_op->fit_into_keep_ratio(render_width / BLANK_PADDING_RATIO, render_height / BLANK_PADDING_RATIO);
        pen_op->move_center_to(Vec2{render_width, render_height} / 2);
    }
    auto buffer = pen_op_to_video(pen_op.get(), PenOpPaintParam{
            .render_width=render_width,
            .render_height=render_height,
            .stroke_width=STROKE_WIDTH,
            .frame_width=frame_width,
            .frame_height=frame_height,
            .stroke_color=0xFFFFFFFF,
            .bg_color=0xFF000000,
    });
//    std::ofstream out("/mnt/d/TEMP/1/my_output.mp4", std::ios::out | std::ios::binary);
    std::ofstream out("/tmp/1/my_output.mp4", std::ios::out | std::ios::binary);
//    std::ofstream out("/tmp/1/my_output.mkv", std::ios::out | std::ios::binary);
    out.write((char *) buffer.data(), buffer.size());
    out.close();
}
