#include "az/chem/utils.h"
#include "openbabel_impl.h"

using namespace az::chem;

std::string Utils::convert_format(
        const std::string_view &from_content,
        const std::string_view &from_format,
        const std::string_view &to_format
) {
    return az::chem::impl::openbabel::convert_format(from_content, from_format, to_format);
}