#include "az/core/init_spdlog.h"

static const bool STATIC_INIT = []() {
    return az::core::setup_spdlog();
}();
