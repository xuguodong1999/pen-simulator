#include "az/core/encoding.h"

#include <spdlog/spdlog.h>

#include <codecvt>
#include <locale>
#include <sstream>

using namespace az;

UCharType az::convert_unicode_hex_to_str(std::string_view sv) {
    UBufType buf = convert_unicode_hex_to_buffer(sv);
    return convert_unicode_buffer_to_str(buf);
}

UBufType az::convert_unicode_hex_to_buffer(std::string_view sv) {
    std::stringstream ss;
    ss << std::hex << sv;
    UBufType buf;
    ss >> buf;
    return buf;
}

UCharType az::convert_unicode_buffer_to_str(const UBufType &buf) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(buf);
}

std::wstring az::convert_unicode_string_to_wstring(std::string_view sv) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(sv.data());
}

std::string az::convert_wstring_to_unicode_string(const std::wstring_view &sv) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(sv.data());
}
