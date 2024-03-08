#pragma once

#include "az/math/config.h"
#include "az/core/config.h"

#include <boost/multiprecision/cpp_dec_float.hpp>

namespace az::math {
    /**
     * generate latex like:
     *      1.2
     * x    3.4
     * ---------
     *      4 8
     *    3 6
     * ---------
     *    4.0 8
     */
    struct AZMATH_EXPORT MultiplyDraft {
        static LabelType generate(
                std::string_view input_1,
                std::string_view input_2,
                std::vector<LabelType> *dst_lines = nullptr
        );
    };
}
