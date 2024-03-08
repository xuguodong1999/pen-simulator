#include "az/pen/pen_stroke.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <string>

using namespace az;
using namespace az::pen;

TEST(test_pen, simple_stroke) {
    PenStroke stroke;
    Box2 rect{
            Vec2{0, 0},
            Vec2{100, 100},
    };
    Vec2Array pts = {
            rect.min(),
            rect.max(),
    };
    size_t i = 0;
    stroke.set_data(pts.size(), [&](auto &x, auto &y) {
        x = pts[i].x();
        y = pts[i].y();
        i++;
    });
    EXPECT_FLOAT_EQ(rect.volume(), stroke.PenOp::bbox().volume());
}