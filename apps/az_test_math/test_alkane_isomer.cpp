#include "az/math/alkane_isomer_counter.h"
#include "az/math/alkane_isomer.h"
#include "az/core/utils.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <vector>

TEST(test_math, polya_algorithm) {
    // https://en.wikipedia.org/wiki/P%C3%B3lya_enumeration_theorem
    std::vector<uint64_t> alkanes = {0, 1, 1, 1, 2, 3, 5, 9, 18, 35};
    std::vector<uint64_t> radicals = {0, 1, 1, 2, 4, 8, 17, 39, 89, 211};
    az::math::PolyaIsomerCounter counter;
    for (int i = 1; i < 10; i++) {
        auto[a, r] = counter.count_alkane_radical(i);
        EXPECT_EQ(a, alkanes[i]);
        EXPECT_EQ(r, radicals[i]);
    }
}

TEST(test_math, print_isomer_size_into_array) {
    az::math::PolyaIsomerCounter counter;
    for (int i = 1; i <= 32; i++) {
        auto[a, r] = counter.count_alkane_radical(i);
        std::cerr << a << ",";
    }
}

TEST(test_math, count_isomer_size) {
    az::math::PolyaIsomerCounter counter;
    double last_count = 1;
    for (int i = 1; i <= 32; i++) {
        auto[a, r] = counter.count_alkane_radical(i);
        double byte_n = a.convert_to<double>() * sizeof(az::math::SmiHashType);
//        const double stage = 1000;
        const double stage = 1024;
        SPDLOG_INFO("alkanes {} count {} about {} {} with factor {}", i, a,
                    byte_n > stage * stage ? byte_n / stage / stage : byte_n > stage ? byte_n / stage : byte_n,
                    byte_n > stage * stage ? "MB" : byte_n > stage ? "KB" : "B",
                    a.convert_to<double>() / last_count
        );
        last_count = a.convert_to<double>();
    }
}

TEST(test_math, greedy_algorithm_cost) {
    // (21) cost 2352.586 ms
    // (22) cost 5456.606 ms
    // (23) cost 19335.943 ms
    az::math::AlkaneIsomerUtil::get_isomers_sync(22);
}

TEST(test_math, greedy_algorithm) {
    az::math::PolyaIsomerCounter counter;
    for (int8_t i = 1; i <= 20; i++) {
        auto smi_list = az::math::AlkaneIsomerUtil::get_isomers_sync(i);
        auto a_greedy = smi_list.size();
        auto[a, r] = counter.count_alkane_radical(i);
        EXPECT_EQ(a, a_greedy);
    }
}

TEST(test_math, alkane_compression) {
    az::math::AlkaneIsomerUtil::compress_isomers(
            az::get_dataset_root("171860633/alkanes-2024-0426")
    );
}