#include "az/data/gsm8k_reader.h"

#include <spdlog/spdlog.h>

#include <gtest/gtest.h>

static void run() {
    using namespace az;
    using namespace az::data;
    Gsm8kReader reader;
}

TEST(test_data, read_gsm8k) {
    run();
}
