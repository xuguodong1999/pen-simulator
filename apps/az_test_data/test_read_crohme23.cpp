#include "az/data/inkml_reader.h"
#include "az/pen/pen_op.h"

#include "utils.h"

#include <spdlog/spdlog.h>

#include <gtest/gtest.h>

const size_t TOTAL = 60;
const int WIDTH = 1024;
const int HEIGHT = 512;
const float STROKE_WIDTH = 6;
const float ROTATE_DEG = 7.5;


static void run() {
    using namespace az;
    using namespace az::data;
    InkmlDatasetReader reader;
    reader.sync_load_all(get_dataset_root("WebData_CROHME23"), {
            "WebData_CROHME23_new_v2.3/new_train",
            "WebData_CROHME23_new_v2.3/new_val",
    });
    std::ostringstream out;
    for (int i = 0; i < TOTAL; i++) {
        auto [raw_data, label] = reader.next();
        auto data = *raw_data;    // deep copy
        for (auto &item: data.data) {
            item.rotate_deg(random_float(-ROTATE_DEG, ROTATE_DEG));
        }
        data.fit_into_keep_ratio(WIDTH - 4 * STROKE_WIDTH, HEIGHT - 4 * STROKE_WIDTH);
        draw_with_skia(&data, fmt::format("{}.png", i), WIDTH, HEIGHT, STROKE_WIDTH);
        out << "R\"xxxxx(" << label->c_str() << ")xxxxx\",";
        SPDLOG_INFO("data.len={}, label={}", data.data.size(), label->c_str());
    }
    SPDLOG_INFO(out.str());
}

TEST(test_data, read_crohme23) {
    run();
}
