#include "az/js/qjs_wrapper.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

using namespace az::js;

TEST(test_js, generate_svg) {
    auto qjs = QjsWrapper::get_instance();
    auto simple_svg = qjs->mathjax_tex_to_svg("1+1=2");
    EXPECT_GT(simple_svg.length(), 0);
}

TEST(test_js, print_var) {
    auto qjs = QjsWrapper::get_instance();
    EXPECT_EQ(qjs->mathjax_get_vars_json("i_am_sure_it_is_undefined"), "undefined");
    auto char_range_json = qjs->mathjax_get_vars_json("charRange");
    EXPECT_EQ(char_range_json, "[[65,90],[97,122],[913,937],[945,969],[48,57]]");
}

void test_str2tex(std::string_view unicode) {
    SPDLOG_INFO("unicode={}", unicode);
    auto unicode_buf = QjsWrapper::convert_unicode_str_to_buffer(unicode);
    SPDLOG_INFO("hex=0x{:X}", unicode_buf);
    EXPECT_EQ(unicode, az::convert_unicode_buffer_to_str(unicode_buf));
}

TEST(test_js, str2hex) {
    std::vector<std::string> TEST_CASE = {
            "😅",
            "Ａ",
            "A",
            "ａ",
            "a",
    };
    for (const auto &test_case: TEST_CASE) {
        test_str2tex(test_case);
    }
}
