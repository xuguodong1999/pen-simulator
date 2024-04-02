#include "az/data/gsm8k_reader.h"

#include <spdlog/spdlog.h>

#include <gtest/gtest.h>

TEST(test_data, read_gsm8k) {
    using namespace az;
    using namespace az::data;
    GSM8kReader reader;
    reader.sync_load_all(az::get_dataset_root("huggingface/gsm8k"));
}
