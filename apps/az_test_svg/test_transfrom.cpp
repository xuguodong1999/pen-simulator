#include "az/svg/transform_parser.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <string>

using namespace az;
using namespace az::svg;

TEST(test_svg, simple_transform) {
    TransformMatrix2 identity;
    identity.setIdentity();
    EXPECT_EQ(identity, parse_transform(""));
}