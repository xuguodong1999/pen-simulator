#include "az/data/unicode_shape.h"

#include "az/data/couch_reader.h"
#include "az/data/inkml_reader.h"
#include "az/js/qjs_wrapper.h"

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::data;
using namespace az::js;

namespace az::data::impl::crohme2023 {
    extern std::unordered_map<std::string, UCharType> CHAR_MAP;
}

namespace az::data::impl::unicode {
    extern std::unordered_map<UCharType, UCharType> SIMILAR_CHAR_MAP;
}

namespace az::data::impl::mathjax {
    void traverse_all_labels(const std::function<void(const UCharType &)> &on_label);

    UBufType map_label_by_style(UBufType label_buf);

    std::optional<UCharType> guess_unicode_composition(
            const std::vector<std::string_view> &svs);
}

void az::data::traverse_all_mathjax_labels(const std::function<void(const UCharType &)> &on_label) {
    az::data::impl::mathjax::traverse_all_labels(on_label);
}

std::optional<UCharType> az::data::guess_unicode_composition(
        const std::vector<std::string_view> &svs) {
    return az::data::impl::mathjax::guess_unicode_composition(svs);
}

std::optional<UCharType> az::data::map_unicode_by_shape(const UCharType &wide_word) {
    const auto &map = az::data::impl::unicode::SIMILAR_CHAR_MAP;
    auto maybe_label = map_unicode_by_label(wide_word);
    if (!maybe_label) { return std::nullopt; }
    auto &label = maybe_label.value();
    auto it = map.find(label);
    if (it == map.end()) { return label; }
    return it->second;
}

std::optional<UCharType> az::data::map_unicode_by_label(const UCharType &wide_word) {
    UBufType unicode_buf;
    try {
        unicode_buf = QjsWrapper::convert_unicode_str_to_buffer(wide_word);
    } catch (std::exception &e) {
        SPDLOG_ERROR("az::data::map_unicode_by_label({}): {}", wide_word, e.what());
        return std::nullopt;
    }
    unicode_buf = az::data::impl::mathjax::map_label_by_style(unicode_buf);
    return az::convert_unicode_buffer_to_str(unicode_buf);
}

std::optional<UCharType> az::data::map_name_to_label(const std::string &name) {
    const auto &map = az::data::impl::crohme2023::CHAR_MAP;
    auto it = map.find(name);
    if (it == map.end()) { return std::nullopt; }
    return it->second;
}
