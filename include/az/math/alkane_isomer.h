#pragma once

#include "az/math/config.h"
#include "az/core/config.h"
#include <cstdint>
#include <functional>

namespace az::math {
    using SmiHashType = uint64_t;

    struct AZMATH_EXPORT AlkaneIsomerUtil {
        /**
         * generate non-optical alkane isomers below 32-C
         */
        static std::vector<SmiHashType> get_isomers_sync(int8_t count);

        static LabelType hash_to_smi(SmiHashType smi_hash);

        static std::vector<LabelType> hash_to_smi_batched(const std::vector<SmiHashType> &smi_hash_list);
    };
}
