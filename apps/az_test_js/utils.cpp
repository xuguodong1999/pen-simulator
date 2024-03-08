#include "./utils.h"
#include "az/js/qjs_wrapper.h"
#include <spdlog/spdlog.h>
#include <chrono>

void az::test::js::mathjax_svg_generation() {
#ifndef __EMSCRIPTEN__ // FIXME: RangeError: Maximum call stack size exceeded
    const char *tex = R"xxxxx(
\begin{pmatrix}
1 & 2 \\
3 & 4
\end{pmatrix}
)xxxxx";
    const auto beg = std::chrono::high_resolution_clock::now();
    std::string svg = az::js::QjsWrapper::get_instance()->mathjax_tex_to_svg(tex);
    const auto end = std::chrono::high_resolution_clock::now();
    SPDLOG_INFO("cost {:.3f} ms",
                std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count() / 1000.f);
#endif
}
