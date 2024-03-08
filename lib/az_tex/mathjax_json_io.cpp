#include "mathjax_json_io.h"
#include "rapidjson_archiver.h"

#include <spdlog/spdlog.h>

#include <type_traits>
#include <iostream>

using namespace az;
using namespace az::tex::impl;
using namespace az::tex::mathjax::impl;

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define SERIALIZE_MEMBER_BY_NAME(ar, bean, x, name) \
    ar.Member(STRINGIFY(name)) & bean.x;

#define SERIALIZE_MEMBER(ar, bean, x) \
    SERIALIZE_MEMBER_BY_NAME(ar, bean, x, x)

#define SERIALIZE_MEMBER_AR_BEAN(x)  SERIALIZE_MEMBER(ar, bean, x)

#define TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, x, name) \
    if(!ar.IsReader || ar.HasMember(STRINGIFY(name))) { \
        SERIALIZE_MEMBER_BY_NAME(ar, bean, x, name) \
    }

#define TRY_SERIALIZE_MEMBER(ar, bean, x) \
    TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, x, x)

#define TRY_SERIALIZE_MEMBER_AR_BEAN(x)  TRY_SERIALIZE_MEMBER(ar, bean, x)

template<typename T>
concept is_output_v = std::is_same_v<T, JsonWriter>;
template<typename T>
concept is_input_v = std::is_same_v<T, JsonReader>;
template<typename T>
concept is_archiver_v=is_output_v<T> || is_input_v<T>;
template<typename T, typename Base>
concept is_const_or_non_const_v=std::is_same_v<T, Base &> || std::is_same_v<T, const Base &>;
namespace az::tex::mathjax::impl {
/// common serialization impl
    template<is_archiver_v Archiver, typename Bean>
    requires is_const_or_non_const_v<Bean, NodeAttr>
    Archiver &archive(Archiver &ar, Bean &&bean) {
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, class_, class)
        TRY_SERIALIZE_MEMBER_AR_BEAN(d)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, data_c, data-c)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, data_mjx_texclass, data-mjx-texclass)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, data_mml_node, data-mml-node)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, data_mml_text, data-mml-text)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, data_variant, data-variant)
        TRY_SERIALIZE_MEMBER_AR_BEAN(display)
        TRY_SERIALIZE_MEMBER_AR_BEAN(fill)
        TRY_SERIALIZE_MEMBER_AR_BEAN(focusable)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, font_family, font-family)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, font_size, font-size)
        TRY_SERIALIZE_MEMBER_AR_BEAN(height)
        TRY_SERIALIZE_MEMBER_AR_BEAN(id)
        TRY_SERIALIZE_MEMBER_AR_BEAN(jax)
        TRY_SERIALIZE_MEMBER_AR_BEAN(role)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, stroke_width, stroke-width)
        TRY_SERIALIZE_MEMBER_AR_BEAN(stroke)
        TRY_SERIALIZE_MEMBER_AR_BEAN(style)
        TRY_SERIALIZE_MEMBER_AR_BEAN(transform)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, view_box, viewBox)
        TRY_SERIALIZE_MEMBER_AR_BEAN(width)
        TRY_SERIALIZE_MEMBER_AR_BEAN(x)
        TRY_SERIALIZE_MEMBER_AR_BEAN(x1)
        TRY_SERIALIZE_MEMBER_AR_BEAN(x2)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, xlink_href, xlink:href)
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, xmlns_xlink, xmlns:xlink)
        TRY_SERIALIZE_MEMBER_AR_BEAN(xmlns)
        TRY_SERIALIZE_MEMBER_AR_BEAN(y)
        TRY_SERIALIZE_MEMBER_AR_BEAN(y1)
        TRY_SERIALIZE_MEMBER_AR_BEAN(y2)
        ar.EndObject();
        return ar;
    }

    template<is_archiver_v Archiver, typename Bean>
    requires is_const_or_non_const_v<Bean, NodeMeta>
    Archiver &archive(Archiver &ar, Bean &&bean) {
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_AR_BEAN(attributes)
        TRY_SERIALIZE_MEMBER_AR_BEAN(kind)
        TRY_SERIALIZE_MEMBER_AR_BEAN(value)
//        SPDLOG_INFO("bean.kind={}, bean.value={}", bean.kind, bean.value);
        ar.EndObject();
        return ar;
    }

    template<is_archiver_v Archiver, typename Bean>
    requires is_const_or_non_const_v<Bean, Node>
    Archiver &archive(Archiver &ar, Bean &&bean) {
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_AR_BEAN(label)
        TRY_SERIALIZE_MEMBER_AR_BEAN(metadata)
        ar.EndObject();
        return ar;
    }

    template<is_archiver_v Archiver, typename Bean>
    requires is_const_or_non_const_v<Bean, EdgeMeta>
    Archiver &archive(Archiver &ar, Bean &&bean) {
        ar.StartObject();
        (void) (bean);
        ar.EndObject();
        return ar;
    }

    template<is_archiver_v Archiver, typename Bean>
    requires is_const_or_non_const_v<Bean, Edge>
    Archiver &archive(Archiver &ar, Bean &&bean) {
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_AR_BEAN(label)
        TRY_SERIALIZE_MEMBER_AR_BEAN(directed)
        TRY_SERIALIZE_MEMBER_AR_BEAN(relation)
        TRY_SERIALIZE_MEMBER_AR_BEAN(source)
        TRY_SERIALIZE_MEMBER_AR_BEAN(target)
        TRY_SERIALIZE_MEMBER_AR_BEAN(metadata)
        ar.EndObject();
        return ar;
    }

    template<is_archiver_v Archiver, typename Bean>
    requires is_const_or_non_const_v<Bean, OutputMeta>
    Archiver &archive(Archiver &ar, Bean &&bean) {
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_BY_NAME(ar, bean, tex, label)
        ar.EndObject();
        return ar;
    }

/// reader
    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, NodeAttr &bean) {
        return archive(ar, bean);
    }

    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, NodeMeta &bean) {
        return archive(ar, bean);
    }

    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, Node &bean) {
        return archive(ar, bean);
    }

    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, EdgeMeta &bean) {
        return archive(ar, bean);
    }

    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, Edge &bean) {
        return archive(ar, bean);
    }

    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, OutputMeta &bean) {
        return archive(ar, bean);
    }

    template<is_input_v Archiver>
    Archiver &operator&(Archiver &ar, Output &bean) {
        ar.StartObject();
        ar.Member(STRINGIFY(graph));
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_AR_BEAN(label)
        TRY_SERIALIZE_MEMBER_AR_BEAN(directed)
        TRY_SERIALIZE_MEMBER_AR_BEAN(type)
        TRY_SERIALIZE_MEMBER_AR_BEAN(metadata)

        { // deserialize array
            ar.Member(STRINGIFY(edges));
            size_t count = 0;
            ar.StartArray(&count);
            bean.edges.resize(count);
            for (auto &node: bean.edges) {
                ar & node;
            }
            ar.EndArray();
        }
        { // deserialize object
            ar.Member(STRINGIFY(nodes));
            ar.StartObject();
            ar.TraverseAllMembers([&](const std::string &k) {
                ar.Member(k.c_str());
                Node node;
                ar & node;
                bean.nodes.insert({k, std::move(node)});
            });
            ar.EndObject();
        }
        return ar.EndObject();
    }

    /// writer
    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const NodeAttr &bean) {
        return archive(ar, bean);
    }

    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const NodeMeta &bean) {
        return archive(ar, bean);
    }

    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const Node &bean) {
        return archive(ar, bean);
    }

    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const EdgeMeta &bean) {
        return archive(ar, bean);
    }

    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const Edge &bean) {
        return archive(ar, bean);
    }

    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const OutputMeta &bean) {
        return archive(ar, bean);
    }

    template<is_output_v Archiver>
    Archiver &operator&(Archiver &ar, const Output &bean) {
        ar.StartObject();
        ar.Member(STRINGIFY(graph));
        ar.StartObject();
        TRY_SERIALIZE_MEMBER_AR_BEAN(label)
        TRY_SERIALIZE_MEMBER_AR_BEAN(type)
        TRY_SERIALIZE_MEMBER_AR_BEAN(directed)
        TRY_SERIALIZE_MEMBER_AR_BEAN(metadata)
        { // serialize array
            ar.Member(STRINGIFY(edges));
            size_t count;
            ar.StartArray(&count);

            for (auto &node: bean.edges) {
                ar & node;
            }
            ar.EndArray();
        }
        { // serialize object
            ar.Member(STRINGIFY(nodes));
            ar.StartObject();
            for (auto &[key, node]: bean.nodes) {
                ar.Member(key.c_str());
                ar & node;
            }
            ar.EndObject();
        }
        ar.EndObject(); // graph end
        ar.EndObject(); // json end
        return ar;
    }

}

/// wrapper
Output Output::from_json(std::string_view json) {
    Output bean;
    JsonReader reader(json.data());
    reader & bean;
    return bean;
}

std::ostream &operator<<(std::ostream &out, const Output &bean) {
    JsonWriter writer;
    writer & bean;
    out << writer.GetString();
    return out;
}
