#include "inkml_raw_item.h"
#include "az/data/inkml_reader.h"
#include "inkml_raw_item.h"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <filesystem>

#include <optional>

using namespace az;
using namespace az::data;
using namespace az::pen;
using namespace az::data::impl;

namespace fs = std::filesystem;
using boost::property_tree::ptree;

template<>
struct fmt::formatter<ptree> {
    static constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const ptree &root, FormatContext &ctx) -> decltype(ctx.out()) {
        std::ostringstream oss;
        auto settings = boost::property_tree::xml_writer_make_settings<std::string>();
        write_xml(oss, root, settings);
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};

template<typename T>
struct fmt::formatter<std::vector<T>> {
    static constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const std::vector<T> &vec, FormatContext &ctx) -> decltype(ctx.out()) {
        std::ostringstream oss;
        for (auto &v: vec) {
            oss << v << ",";
        }
        return fmt::format_to(ctx.out(), "{}", oss.str());
    }
};

static std::optional<ptree::data_type> get_attr_pt(ptree &root, const ptree::key_type &key) {
    auto maybe_attr = root.get_child_optional("<xmlattr>");
    if (!maybe_attr) { return std::nullopt; }
    auto maybe_key = maybe_attr->get_child_optional(key);
    if (!maybe_key) { return std::nullopt; }
    return maybe_key->data();
}

static void load_trace_group_pt(InkmlRawItem &result, ptree &root) {
    std::vector<std::string> ids;
    std::string label;
    std::string mml_label;
    for (auto &v: root) {
        auto &[key, sub_node] = v;
        if ("traceGroup" == key) {
            load_trace_group_pt(result, sub_node);
        } else if ("annotation" == key) {
            label = sub_node.data();
            // SPDLOG_INFO("trace_group label:{}", label);
        } else if ("traceView" == key) {
            auto ref = get_attr_pt(sub_node, "traceDataRef");
            if (ref) {
                // SPDLOG_INFO("trace_group id:{}", ref.value());
                ids.push_back(ref.value());
            }
        } else if ("annotationXML" == key) {
            auto href = get_attr_pt(sub_node, "href");
            if (href) {
                mml_label = href.value();
            }
        }
    }
    if (!ids.empty()) {
        result.trace_groups.emplace_back(label, std::move(ids));
    }
    if (!mml_label.empty()) {
        result.mml_index_map[mml_label] = result.trace_groups.size() - 1;
    }
}

static void load_inkml_pt(InkmlRawItem &result, ptree &ink_node) {
    for (auto &v: ink_node) {
        auto &[key, sub_node] = v;
        if ("annotationXML" == key) {
            auto encoding = get_attr_pt(sub_node, "encoding");
            if (encoding && encoding.value() == "Content-MathML") {
                result.mml = sub_node.data();
            }
        } else if ("annotation" == key) {
            auto type = get_attr_pt(sub_node, "type");
            if (type && type.value() == "truth") {
                // SPDLOG_INFO("latex={}", sub_node.data());
                result.latex = sub_node.data();
            }
        } else if ("trace" == key) {
            auto id = get_attr_pt(sub_node, "id");
            if (id) {
                // SPDLOG_INFO("trace[{}]={}", id.value(), sub_node.data());
                PenStroke path;
                std::vector<std::string_view> pts;
                std::string buf = sub_node.data();
                boost::algorithm::split(pts, sub_node.data(), boost::is_any_of(","));
                size_t i = 0;
                path.set_data(pts.size(), [&](ScalarType &x, ScalarType &y) {
                    // horribly slow...
                    // std::istringstream in(pts[i++].data());
                    // in >> x >> y;
                    auto &sv = pts[i++];
                    size_t offset;
                    x = std::stof(sv.data(), &offset);
                    y = std::stof(sv.substr(offset).data());
                });
                result.id_path_map[id.value()] = std::move(path);
            }
        } else if ("traceGroup" == key) {
            load_trace_group_pt(result, sub_node);
        }
    }
}

InkmlItem InkmlRawItem::read(
        std::istream &in,
        const std::function<void(const PenChar &, const LabelType &)> &on_char
) {
    ptree pt;
    try {
        //"	&quot;
        //'	&apos;
        //<	&lt;
        //>	&gt;
        //&	&amp;
        read_xml(in, pt);
    } catch (std::exception &e) {
        SPDLOG_ERROR(e.what());
        throw e;
    }
    auto maybe_ink_node = pt.get_child_optional("ink");
    if (!maybe_ink_node) {
        throw std::runtime_error("ink tag not found");
    }
    load_inkml_pt(*this, maybe_ink_node.value());
    InkmlItem item;
    item.latex = this->latex;
    auto &chars = item.data.data;
    if (!trace_groups.empty()) {
        // manually collected data has valid traceGroup
        chars.reserve(trace_groups.size());
        for (auto &[label, ids]: trace_groups) {
            PenChar c;
            auto &paths = c.data;
            paths.reserve(ids.size());
            for (auto &id: ids) {
                paths.push_back(std::move(id_path_map[id]));
            }
            on_char(c, label);
            chars.push_back(std::move(c));
        }
    } else {
        // syntactic data has no traceGroup
        chars.reserve(id_path_map.size());
        for (auto &[id, path]: id_path_map) {
            PenChar c;
            c.data = {std::move(path)};
            on_char(c, "unknown");
            chars.push_back(std::move(c));
        }
    }
    return item;
}
