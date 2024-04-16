#include "az/data/couch_reader.h"
#include "az/pen/pen_op.h"

#include "utils.h"

#include <spdlog/spdlog.h>

#include <gtest/gtest.h>

const size_t TOTAL = 60;
const int WIDTH = 512;
const int HEIGHT = 512;
const float STROKE_WIDTH = 6;
const float ROTATE_DEG = 15;

static void run() {
    using namespace az;
    using namespace az::data;
    CouchDatasetReader reader;

    reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
            "Couch_Digit_195",
            "Couch_GB1_188",
            "Couch_GB2_195",
            "Couch_Letter_195",
            "Couch_Symbol_130"
    });
    for (int i = 0; i < TOTAL; i++) {
        auto [raw_data, label] = reader.next();
//        auto label = std::make_shared<UCharType>("好");
//        auto raw_data = reader.select(*label);
        auto data = *raw_data;
        data.rotate_deg(random_float(-ROTATE_DEG, ROTATE_DEG));
        data.fit_into_keep_ratio(WIDTH - 4 * STROKE_WIDTH, HEIGHT - 4 * STROKE_WIDTH);
        draw_with_skia(&data, fmt::format("{}.png", i), WIDTH, HEIGHT, STROKE_WIDTH);
        SPDLOG_INFO("data.len={}, label={}", data.data.size(), label->c_str());
    }
}

TEST(test_data, read_couch) {
    run();
}
