#include "az/svg/path_parser.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <string>

using namespace az;
using namespace az::svg;

TEST(test_svg, simple_rect) {
    auto path = parse_path("M0 0L1 0L1 1L0 1Z");
    auto rect = path.bbox();
    Box2 truth{Vec2{0, 0}, Vec2{1, 1}};
    EXPECT_FLOAT_EQ(rect.volume(), truth.volume());
}