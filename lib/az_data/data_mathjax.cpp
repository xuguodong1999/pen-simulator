#include "az/core/encoding.h"
#include "az/data/unicode_shape.h"

#include "az/data/couch_reader.h"
#include "az/data/inkml_reader.h"
#include "az/js/qjs_wrapper.h"
#include "az/core/encoding.h"

#include <spdlog/spdlog.h>
#include <boost/property_tree/json_parser.hpp>

#include <unordered_set>

using namespace az;
using namespace az::data;
using namespace az::js;

// [
//      [Upper-case alpha, Lower-case alpha, Upper-case Greek, Lower-case Greek, Numbers],
//      ...
// ]
// see ts/output/common/FontData.ts
static std::vector<std::array<UBufType, 5>> VARIANT_SMP = []() {
    boost::property_tree::ptree pt;
    {
        auto qjs = QjsWrapper::get_instance(false);
        std::istringstream in(qjs->mathjax_get_vars_json("styledCharRange"));
        boost::property_tree::read_json(in, pt);
    }
    std::vector<std::array<UBufType, 5>> result;
    result.reserve(pt.size());
    for (auto &[key1, value1]: pt) {
        std::array<UBufType, 5> item{};
        size_t i = 0;
        for (auto &[key2, value2]: value1) {
            if (i >= item.size()) {
                SPDLOG_ERROR("unexpected array size: {}", value1.size());
                break;
            }
            item[i++] = value2.get_value<UBufType>();
        }
//        SPDLOG_INFO("[{},{}, {},{}, {}]", item[0], item[1], item[2], item[3], item[4]);
        result.push_back(item);
    }
    return result;
}();

//[
//      [65,90],    // Upper-case alpha
//      [97,122],   // Lower-case alpha
//      [913,937],  // Upper-case Greek
//      [945,969],  // Lower-case Greek
//      [48,57]     // Numbers
//
// see ts/output/common/FontData.ts
static std::vector<std::array<UBufType, 2>> SMP_RANGES = []() {
    boost::property_tree::ptree pt;
    {
        auto qjs = QjsWrapper::get_instance(false);
        std::istringstream in(qjs->mathjax_get_vars_json("charRange"));
        boost::property_tree::read_json(in, pt);
    }
    //[2024-02-04 14:19:14.895] [info] [test_qjs.cpp:19] char_range_json=
    //[[65,90],[97,122],[913,937],[945,969],[48,57]]
    std::vector<std::array<UBufType, 2>> result;
    result.reserve(pt.size());
    for (auto &[key1, value1]: pt) {
        std::array<UBufType, 2> item{};
        size_t i = 0;
        for (auto &[key2, value2]: value1) {
            if (i >= item.size()) {
                SPDLOG_ERROR("unexpected array size: {}", value1.size());
                break;
            }
            item[i++] = value2.get_value<UBufType>();
        }
//        SPDLOG_INFO("[{},{}]", item[0], item[1]);
        result.push_back(item);
    }
    return result;
}();

// checkout from http://www.hcii-lab.net/data/hcclib/hcclib_chn.htm
static std::vector<std::array<UBufType, 4>> CHAR_SECTIONS = []() {
    // <(start,end), (normal_start,normal_end)>
    auto qjs = az::js::QjsWrapper::get_instance(false);
    std::vector<std::array<UBufType, 4>> result = {
            // couch
            {
                    QjsWrapper::convert_unicode_str_to_buffer("！"),
                    QjsWrapper::convert_unicode_str_to_buffer("｝"),
                    QjsWrapper::convert_unicode_str_to_buffer("!"),
                    QjsWrapper::convert_unicode_str_to_buffer("}"),
            },
    };
//    auto &[a, b, c, d] = result[0];
//    SPDLOG_INFO("{},{},{},{}", a, b, c, d);
    return result;
}();

static std::string make_composition_key(const std::vector<std::string_view> &compositions) {
    size_t buffer_len = 0;
    for (const auto &str: compositions) {
        buffer_len += str.length();
    }
    std::string comp_key;
    comp_key.reserve(buffer_len);
    for (auto &sv: compositions) {
        comp_key += sv;
    }
    return comp_key;
}

static std::unordered_map<UCharType, std::vector<UCharType>> COMPOSITION_MAP = []() {
    std::unordered_map<UCharType, std::vector<UCharType>> result;
    auto qjs = QjsWrapper::get_instance(false);
    boost::property_tree::ptree composed_char_map;
    std::istringstream in(qjs->mathjax_get_vars_json("composedCharMap"));
    boost::property_tree::read_json(in, composed_char_map);
    for (auto &[name, value]: composed_char_map) {
        if (result.contains(name)) {
            SPDLOG_ERROR("conflict key \"{}\" in composedCharMap, skip...", name);
            continue;
        }
        std::vector<UCharType> compositions;
        compositions.reserve(value.size());
        for (auto &[_, child_name]: value) {
            const auto &child_char = child_name.data();
            compositions.push_back(child_char);
        }
//        std::stringstream out;
//        out << name << " is made up of [";
//        for (auto &comp: compositions) { out << comp << ","; }
//        out << "]";
//        SPDLOG_INFO("composedCharMap: {}", out.str());
        result.insert({name, std::move(compositions)});
    }
    return result;
}();

namespace az::data::impl::mathjax {
    void traverse_all_labels(const std::function<void(const UCharType &)> &on_label) {
        auto qjs = QjsWrapper::get_instance(false);
        // https://stackoverflow.com/questions/54175811/read-vector-of-strings-value-from-json-file-in-c-using-boost
        {
            boost::property_tree::ptree char_name_map;
            std::istringstream in(qjs->mathjax_get_vars_json("charNameMap"));
            boost::property_tree::read_json(in, char_name_map);
            for (auto &[name, value]: char_name_map) {
                on_label(value.data());
            }
        }
        {
            boost::property_tree::ptree composed_char_map;
            std::istringstream in(qjs->mathjax_get_vars_json("composedCharMap"));
            boost::property_tree::read_json(in, composed_char_map);
            for (auto &[name, value]: composed_char_map) {
                std::vector<UCharType> compositions;
                compositions.reserve(value.size());
                on_label(name);
                for (auto &[_, child_name]: value) {
                    on_label(child_name.data());
                }
            }
        }
        {
            boost::property_tree::ptree variants_map;
            auto variants_json = qjs->mathjax_get_vars_json("variants");
            std::istringstream in(variants_json);
            boost::property_tree::read_json(in, variants_map);
            for (auto &[font_type, font_value]: variants_map) {
                for (auto &[char_name, char_path]: font_value) {
                    on_label(char_name);
                }
            }
        }
    }

    std::optional<UCharType> guess_unicode_composition(
            const std::vector<std::string_view> &svs) {
        // TODO: reduce complexity
        for (auto &[label, compositions]: COMPOSITION_MAP) {
            if (compositions.size() != svs.size()) { continue; }
            for (size_t i = 0; i < svs.size(); i++) {
                if (compositions[i] != svs[i]) { continue; }
            }
            return label;
        }
        return std::nullopt;
    }

    UBufType map_label_by_style(UBufType label_buf) {
        // 1. we map alpha, greek and numbers
        for (auto &smp_type: VARIANT_SMP) {
            for (size_t i = 0; i < smp_type.size(); i++) {
                const auto &start = smp_type[i];
                const auto &smp_range = SMP_RANGES[i];
                const auto end = start + smp_range[1] - smp_range[0];
                if (start <= label_buf && label_buf <= end) {
                    label_buf -= (start - smp_range[0]);
                    goto BREAK;
                }
            }
        }
        BREAK:;
        // 2. we map by sections
        for (auto &[start, end, normal_start, normal_end]: CHAR_SECTIONS) {
            if (start <= label_buf && label_buf <= end) {
                label_buf -= (start - normal_start);
                break;
            }
        }
        return label_buf;
    }
}
