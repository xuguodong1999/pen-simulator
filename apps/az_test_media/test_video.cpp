#include "az/media/data_output.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/pen/pen_graph.h"

#include <gtest/gtest.h>

using namespace az;
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
    pen_op_to_video(std::dynamic_pointer_cast<az::pen::PenOp>(pen_op).get(), PenOpPaintParam{
            .frame_width=512,
            .frame_height=512,
            .stroke_color=0xFF,
            .bg_color=0x0,
    });
}
