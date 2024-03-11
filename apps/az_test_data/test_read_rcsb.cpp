#include "az/data/rcsb_reader.h"

#include <spdlog/spdlog.h>

#include <gtest/gtest.h>

static void run() {
    using namespace az;
    using namespace az::data;
    RCSBReader reader;
    reader.sync_load_all(get_dataset_root("rcsb"), "mmCIF");
}

TEST(test_data, read_rcsb) {
    run();
}
