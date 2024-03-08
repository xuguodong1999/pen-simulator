#include "./utils.h"

#include <gtest/gtest.h>
#include <taskflow/taskflow.hpp>

TEST(test_tex, mathjax_svg_generation_multithreading) {
    tf::Taskflow taskflow;
    for (int i = 1; i < 4; i++) {
        taskflow.for_each_index(0, 3 * i, 1, [](int index) {
            az::test::js::mathjax_svg_generation();
        });
        tf::Executor(i).run(taskflow).get();
    }
}
