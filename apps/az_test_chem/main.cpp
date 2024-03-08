#include "az/core/init_spdlog.h"
#include "az/init_openbabel.h"
#include <gtest/gtest.h>

static const bool STATIC_INIT = []() {
    if (!az::core::setup_spdlog()) {
        return false;
    }
    return az::init::setup_openbabel();
}();
