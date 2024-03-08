#pragma once

#include "az/core/json_graph.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace az::tex::mathjax::impl {
    struct NodeAttr {
        std::string class_,
                data_c, data_mjx_texclass, data_mml_node, data_mml_text, data_variant,
                display, fill, focusable,
                font_family, font_size,
                id,
                jax, role,
                stroke_width, stroke, style,
                transform, view_box, width, height, x, y, d,
                x1, y1, x2, y2,
                xlink_href, xmlns_xlink, xmlns;
    };
    struct NodeMeta {
        NodeAttr attributes;
        std::string kind;
        std::string value;
    };
    struct Node : public JsonGraphNode<NodeMeta> {
    };
    struct EdgeMeta {
    };
    struct Edge : public JsonGraphEdge<EdgeMeta> {
    };
    struct OutputMeta {
        std::string tex;
    };

    struct Output : public JsonGraph<Node, Edge, OutputMeta> {
        static Output from_json(std::string_view json);
    };

    std::ostream &operator<<(std::ostream &, const Output &);
}
