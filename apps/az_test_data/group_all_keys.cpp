#include "az/data/couch_reader.h"
#include "az/data/inkml_reader.h"
#include "az/js/qjs_wrapper.h"
#include "az/data/unicode_shape.h"

#include <spdlog/spdlog.h>

#include <gtest/gtest.h>
#include <unordered_set>

using namespace az;
using namespace az::data;
using namespace az::js;

template<typename T>
requires std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>
static std::string get_string(const T &str) {
    if constexpr (std::is_same_v<std::wstring, T>) {
        return az::convert_wstring_to_unicode_string(str);
    } else {
        return str;
    }
}

template<typename T>
requires std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>
static void insert(std::unordered_set<T> &set, const std::string &key) {
    if constexpr (std::is_same_v<std::wstring, T>) {
        auto w_key = az::convert_unicode_string_to_wstring(key);
        set.insert(std::move(w_key));
    } else {
        set.insert(key);
    }
}


static std::string map_char_to_label(const std::string &s) {
    // 1. we map alpha, greek and numbers
    // 2. we map by sections
    // checkout from http://www.hcii-lab.net/data/hcclib/hcclib_chn.htm
    auto maybe_result = az::data::map_unicode_by_label(s);
    EXPECT_TRUE(maybe_result);
    std::string result = std::move(maybe_result.value());
    // 3. we handle similar chars
    auto maybe_another_label = az::data::map_unicode_by_shape(result);
    if (maybe_another_label) {
        result = maybe_another_label.value();
    }
    // 4. we filter strange chars to white space
    std::unordered_set<std::string> STRANGE_CHARS = {
            " ",
            "",
            "",
            "",
            "",
            "",
            "",
            " ",
            "̀",
            "́",
            "̂",
            "̃",
            "̄",
            "̆",
            "̇",
            "̈",
            "̊",
            "̋",
            "̌",
            "࠘",
            " ",
            " ",
            " ",
            " ",
            " ",
            " ",
            " ",
            " ",
            " ",
            "​",
            "‌",
            "⁠",
            "⁡",
            "⁢",
            "⁣",
            "⁤",
            "𐁲",
            "𐈖",
            "𐈗",
            "𐈢",
            "𐈣",
            "𐈩",
            "𐈰",
            "𐈱",
            "𐈲",
            "𐈳",
            "𐈴",
            "𐈵",
            "𐈶",
            "𐈷",
            "𐈸",
            "𐕴",
            "𐕵",
            "𐕶",
            "𐕷",
            "𐖆",
            "𐖇",
            "𐖈",
            "𐖉",
            "𐖐",
            "𐖑",
            "𐖒",
            "𐖓",
            "񥁹",
            "񥂀",
    };
//    if (STRANGE_CHARS.contains(result)) {
//        result = " ";
//    }
    return result;
}

static void run() {
    std::unordered_set<LabelType> key_set;
    {
        CouchDatasetReader reader;
        reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
//                "Couch_GB1_188",
//                "Couch_GB2_195",
                "Couch_Digit_195",
                "Couch_Letter_195",
                "Couch_Symbol_130"
        });
        auto &keys = reader.get_keys();
        for (auto &key: keys) {
            insert(key_set, key);
        }
    }
    SPDLOG_INFO("total={}", key_set.size());

    az::data::traverse_all_mathjax_labels([&](const UCharType &key) {
        insert(key_set, key);
    });
    SPDLOG_INFO("total={}", key_set.size());

    {
        InkmlDatasetReader reader;
        reader.sync_load_all(get_dataset_root("WebData_CROHME23"), {
                "WebData_CROHME23_new_v2.3/new_train",
                "WebData_CROHME23_new_v2.3/new_val",
        });
        reader.get_keys([&](const LabelType &key) {
            insert(key_set, key);
        });
    }
    SPDLOG_INFO("total={}", key_set.size());

    EXPECT_TRUE(key_set.contains("𝐴"));

    std::unordered_set<UCharType> filtered_key_set;
    for (auto &item: key_set) {
        auto may_from_long_label = map_name_to_label(item);
        const auto &unicode_char = may_from_long_label ? may_from_long_label.value() : item;
        std::string mapped_char = map_char_to_label(unicode_char);
        if (item != mapped_char) {
//            SPDLOG_INFO("map \"{}\" to \"{}\"", item, mapped_char);
        }
        insert(filtered_key_set, mapped_char);
    }
    SPDLOG_INFO("total={}", filtered_key_set.size());
//    if (true) {
    if (false) {
        std::vector<LabelType> all_keys = move_unordered_set_to_vector(filtered_key_set);
        std::sort(all_keys.begin(), all_keys.end());
        std::ostringstream out;
        out << "[";
        for (auto &label: all_keys) {
            out << "\"" << label << "\",";
        }
        out << "]";
        SPDLOG_INFO("buf=\n{}", get_string(out.str()));
    }
}

TEST(test_data, group_key_set) {
    run();
}
