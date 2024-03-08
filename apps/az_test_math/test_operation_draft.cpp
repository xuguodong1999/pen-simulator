#include "az/math/multiply_draft.h"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

static void run_multiple_draft(std::string_view a, std::string_view b) {
    std::string input_1 = R"(1.234)";
    std::string input_2 = R"(5.67)";
    std::vector<az::LabelType> lines;
    auto latex = az::math::MultiplyDraft::generate(a, b, &lines);
    SPDLOG_INFO("latex=\n{}", latex);
    for (auto &line: lines) {
        SPDLOG_INFO("line={}", line);
    }
}

TEST(test_math, multiple_draft) {
    run_multiple_draft("1.234", "5.67");
}

TEST(test_math, multiple_draft2) {
    run_multiple_draft("12.34", "12.34");
}

TEST(test_math, multiple_draft3) {
    run_multiple_draft("0.001", "0.1");
}

TEST(test_math, multiple_draft4) {
    run_multiple_draft("0", "0");
}

TEST(test_math, multiple_draft5) {
    run_multiple_draft("123", "30");
}

TEST(test_math, multiple_draft6) {
    run_multiple_draft("10.3", "4567");
}


TEST(test_math, multiple_draft7) {
    run_multiple_draft("5.07", "6");
}

TEST(test_math, multiple_draft8) {
    run_multiple_draft("86.24", "2.903");
}

TEST(test_math, multiple_draft9) {
    run_multiple_draft("67.3", "7092");
}


TEST(test_math, multiple_draft10) {
    run_multiple_draft("9315", "98");
}

TEST(test_math, multiple_draft11) {
    run_multiple_draft("98.3", "2.7");
}

TEST(test_math, multiple_draft12) {
    run_multiple_draft("734", "42");
}
