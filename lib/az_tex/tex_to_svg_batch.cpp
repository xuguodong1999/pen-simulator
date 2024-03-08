#include "az/tex/api.h"

#include <taskflow/taskflow.hpp>
#include <spdlog/spdlog.h>
#include <thread>
#include <algorithm>

std::vector<std::string> az::tex::convert_tex_to_svg_batch(
        const std::vector<std::string_view> &in_list, unsigned int parallel_num) {
    tf::Taskflow taskflow;
    const auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, parallel_num);
    std::vector<std::string> out_list;
    size_t total = in_list.size();
    out_list.reserve(total);
    out_list.resize(total);
    taskflow.for_each_index(size_t{0}, total, size_t{1}, [&in_list, &out_list](size_t index) {
        try {
            out_list[index] = az::tex::convert_tex_to_svg(in_list[index]);
        } catch (std::exception &e) {
            SPDLOG_ERROR(e.what());
        }
    });
    tf::Executor(thread_num).run(taskflow).get();
    return out_list;
}
