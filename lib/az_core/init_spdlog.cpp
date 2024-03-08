#include "az/core/init_spdlog.h"

#include <spdlog/spdlog.h>

#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#endif

namespace az::core {
    // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags
    bool setup_spdlog() {
#ifdef __ANDROID__
        auto default_logger = spdlog::android_logger_mt("spdlog");
        spdlog::set_default_logger(default_logger);
        spdlog::set_pattern("%v");
#else
        spdlog::set_pattern("%P-%t|%R:%S-%e|%l %s:%# %v");
#endif
        SPDLOG_INFO("spdlog init done.");
        return true;
    }
}

//static const bool SPDLOG_INIT = []() {
//    setup_spdlog()
//    return true;
//}();
