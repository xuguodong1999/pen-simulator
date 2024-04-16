#include "az/core/init_spdlog.h"

#include <gtest/gtest.h>

static const bool STATIC_INIT = []() {
    return az::core::setup_spdlog();
}();

TEST(test_media, hello_world) {
    EXPECT_TRUE(1 + 1 == 2);
}
