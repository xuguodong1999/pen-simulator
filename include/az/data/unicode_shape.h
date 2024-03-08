#pragma once

#include "az/data/config.h"
#include "az/core/config.h"
#include <optional>
#include <string_view>
#include <functional>

namespace az::data {
    AZDATA_EXPORT void traverse_all_mathjax_labels(const std::function<void(const UCharType &)> &on_label);
    // Guess a char from a group of chars.
    //      See mathjax: ts/output/common/fonts/tex/delimiters.ts
    AZDATA_EXPORT std::optional<UCharType> guess_unicode_composition(
            const std::vector<std::string_view> &svs);

    // Now that we have applied clustering method before,
    //      for example, by staring at a lot of unicodes manually or clustering by font pixels similarity,
    //      we made a static map and do the shape mapping here.
    AZDATA_EXPORT std::optional<UCharType> map_unicode_by_shape(const UCharType &wide_word);

    // It is meaningless to distinguish between different styles of a same char,
    //      for example, "VvνⅤⅴ∨⋁". Perhaps pick out "⋁" is helpful, whatever, do language-level mapping here.
    //      Notice: language-level mapping is weaker than shape mapping.
    AZDATA_EXPORT std::optional<UCharType> map_unicode_by_label(const UCharType &wide_word);

    AZDATA_EXPORT std::optional<UCharType> map_name_to_label(const std::string &name);
}