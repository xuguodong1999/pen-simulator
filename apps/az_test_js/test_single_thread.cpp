#include "./utils.h"
#include "az/js/qjs_wrapper.h"
#include <gtest/gtest.h>

using namespace az::js;

TEST(test_js, mathjax_svg_generation_single_thread) {
    az::test::js::mathjax_svg_generation();
}
