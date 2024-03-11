#pragma once

#include "az/chem/config.h"
#include <string>

namespace az::chem {
    struct AZCHEM_EXPORT Utils {
        static std::string convert_format(
                const std::string_view &from_content,
                const std::string_view &from_format,
                const std::string_view &to_format
        );
    };
}
