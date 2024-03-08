#include "az/tex/api.h"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <fstream>

TEST(test_tex, parse_mathjax_json) {
    const char *tex_str = R"xxxxx(
\begin{pmatrix}
 你好 & c_1 & c_1^2 & \cdots & c_1^n \\
 😅 & c_2 & c_2^2 & \cdots & c_2^n \\
 \vdots  & \vdots& \vdots & \ddots & \vdots \\
 1 & c_m & c_m^2 & \cdots & c_m^n \\
\end{pmatrix}
)xxxxx";
    SPDLOG_INFO("tex={}", tex_str);
    auto json = az::tex::convert_tex_to_json(tex_str);
//    SPDLOG_INFO("json={}", json);
//    {
//        std::ofstream ofsm("C:/Users/xgd/Desktop/TEMP.json", std::ios::out);
//        ofsm << json;
//        ofsm.close();
//    }
}
