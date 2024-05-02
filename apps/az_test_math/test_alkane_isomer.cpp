#include "az/math/alkane_isomer_counter.h"
#include "az/math/alkane_isomer.h"
#include "az/core/utils.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <boost/sort/sort.hpp>

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

static void analysis_sorted_alkanes(const std::vector<az::math::SmiHashType> &smi_chunk) {
    for (size_t i = 1; i < smi_chunk.size(); i++) {
        if (smi_chunk[i - 1] >= smi_chunk[i]) {
            SPDLOG_ERROR("error found: {} {}", i - 1, i);
        }
    }
    if (smi_chunk.size() < 1024) { return; }
    using namespace az::math;
    size_t p = (size_t{1} << 33) - (size_t{1} << 31);
    size_t p1 = (size_t{1} << 32) - (size_t{1} << 30);
    size_t p2 = (size_t{1} << 30) - (size_t{1} << 28);
    double bound = 1. * p / (p + p1 + p2);
    double bound1 = 1. * (p + p1) / (p + p1 + p2);
    size_t k_index = std::round(smi_chunk.size() * bound);
    SmiHashType k = smi_chunk[k_index];
    size_t k1_index = std::round(smi_chunk.size() * bound1);
    SmiHashType k1 = smi_chunk[k1_index];

    double r = 1. * (smi_chunk.back() - k) / (smi_chunk.back());
    double r1 = 1. * (smi_chunk.back() - k1) / (smi_chunk.back());
    SPDLOG_INFO("r={:.6f},r1={:.6f}", r, r1);
    // now we check what will happen if delta 0.002 is introduced
    const double delta = 0.002;
    SmiHashType k_up = smi_chunk.back() - std::round(1. * smi_chunk.back() * (r - delta));
    SmiHashType k_down = smi_chunk.back() - std::round(1. * smi_chunk.back() * (r + delta));
    SmiHashType k1_up = smi_chunk.back() - std::round(1. * smi_chunk.back() * (r1 - delta));
    SmiHashType k1_down = smi_chunk.back() - std::round(1. * smi_chunk.back() * (r1 + delta));
    size_t k_up_offset = 0;
    size_t k_down_offset = 0;
    size_t k1_up_offset = 0;
    size_t k1_down_offset = 0;
    while (smi_chunk[k_index + k_up_offset] < k_up) {
        k_up_offset++;
    }
    while (smi_chunk[k_index - k_down_offset] > k_down) {
        k_down_offset++;
    }
    while (smi_chunk[k1_index + k_up_offset] < k1_up) {
        k1_up_offset++;
    }
    while (smi_chunk[k1_index - k1_down_offset] > k1_down) {
        k1_down_offset++;
    }
    SPDLOG_INFO("offset={:.6f},{:.6f},{:.6f},{:.6f}",
                1. * k_up_offset / smi_chunk.size(),
                1. * k_down_offset / smi_chunk.size(),
                1. * k1_up_offset / smi_chunk.size(),
                1. * k1_down_offset / smi_chunk.size()
    );
}

TEST(test_math, c31_bound_test) {
    using namespace az::math;
    SmiHashType smi_chunk_back = 0b11111111111111110000000000000001111111111111110000000000000000;
    // c27 r=0.079349,r1=0.043615
    // c27 r=0.078859,r1=0.043512
    SmiHashType bound = std::round((1. - 0.078) * smi_chunk_back);
    SmiHashType bound1 = std::round((1. - 0.043) * smi_chunk_back);
    SPDLOG_INFO("bound={}, bound1={}", bound, bound1);
    SPDLOG_INFO("ratio={:.6f}, ratio1={:.6f}",
                1. * (smi_chunk_back - bound) / smi_chunk_back,
                1. * (smi_chunk_back - bound1) / smi_chunk_back
    );
}

TEST(test_math, alkane_distribution_analysis) {
    using namespace az;
    using namespace az::math;
    std::string path = az::get_dataset_root("171860633/alkanes-2024-0426/");
    if (!std::filesystem::exists(path)) {
        SPDLOG_ERROR("{} is not an existing directory", path);
        return;
    }
    int8_t end = 0;
    const auto &directory = path;
    SPDLOG_INFO("directory={}", directory);
    while (fs::exists(directory + fmt::format("{}{}", end + 1, ".sort.dat"))) {
        end++;
    }
    end += 1; // end-1 should exist
    for (int8_t i = 1; i < end; i++) {
        const int8_t n = i;
        std::ifstream in;
        in.open(directory + fmt::format("{}{}", n, ".sort.dat"), std::ios::in | std::ios::binary);
        size_t chunk_size = 0;
        in.read(reinterpret_cast<char *>(&chunk_size), sizeof(size_t));
        std::vector<SmiHashType> smi_chunk;
        smi_chunk.reserve(chunk_size);
        smi_chunk.resize(chunk_size);
        in.read(reinterpret_cast<char *>(smi_chunk.data()), sizeof(SmiHashType) * chunk_size);
        SPDLOG_INFO("C {} contains {} alkanes", n, smi_chunk.size());
        analysis_sorted_alkanes(smi_chunk);
    }
}

TEST(test_math, alkane_sort) {
    using namespace az;
    using namespace az::math;
    std::string path = az::get_dataset_root("171860633/alkanes-2024-0426/");
    if (!std::filesystem::exists(path)) {
        SPDLOG_ERROR("{} is not an existing directory", path);
        return;
    }
    int8_t end = 0;
    const auto &directory = path;
    SPDLOG_INFO("directory={}", directory);
    while (fs::exists(directory + fmt::format("{}{}", end + 1, ".dat"))) {
        end++;
    }
    end += 1; // end-1 should exist
    for (int8_t i = 1; i < end; i++) {
        const int8_t n = i;
        std::ifstream in;
        in.open(directory + fmt::format("{}{}", n, ".dat"), std::ios::in | std::ios::binary);
        size_t chunk_size = 0;
        in.read(reinterpret_cast<char *>(&chunk_size), sizeof(size_t));
        std::vector<SmiHashType> smi_chunk;
        smi_chunk.reserve(chunk_size);
        smi_chunk.resize(chunk_size);
        in.read(reinterpret_cast<char *>(smi_chunk.data()), sizeof(SmiHashType) * chunk_size);
        boost::sort::block_indirect_sort(smi_chunk.begin(), smi_chunk.end());

        std::ofstream out(
                directory + fmt::format("{}{}", n, ".sort.dat"), std::ios::out | std::ios::binary);
        out.write(reinterpret_cast<char *>(&chunk_size), sizeof(size_t));
        out.write(reinterpret_cast<char *>(smi_chunk.data()), sizeof(SmiHashType) * chunk_size);
    }
}
