#pragma once

#include "az/core/config.h"
#include <string_view>
#include <string>

namespace az {
    AZCORE_EXPORT UCharType convert_unicode_hex_to_str(std::string_view sv);

    AZCORE_EXPORT UBufType convert_unicode_hex_to_buffer(std::string_view sv);

    AZCORE_EXPORT UCharType convert_unicode_buffer_to_str(const UBufType &buf);

    AZCORE_EXPORT std::wstring convert_unicode_string_to_wstring(std::string_view sv);

    AZCORE_EXPORT std::string convert_wstring_to_unicode_string(const std::wstring_view &sv);
}