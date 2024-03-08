#pragma once

#include "AzCore_export.h"

#include <vector>
#include <string>
#include <memory>

namespace az::core {
}

namespace az {
    // single unicode character buffer
    using UBufType = int32_t;
    // single unicode character
    using UCharType = std::string;
    // label, maybe latex or a single unicode character or something else
    using LabelType = std::string;
    using IndexType = size_t;
    using SizeType = size_t;
    using IdType = uint64_t;
    template<typename T>
    using PointerVec = std::vector <std::shared_ptr<T>>;
}
