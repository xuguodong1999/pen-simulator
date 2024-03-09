#pragma once

#include "az/tex/config.h"
#include "az/core/geometry_def.h"

#include <Eigen/Core>

#include <memory>
#include <string>
#include <optional>
#include <unordered_map>

namespace az::svg {
    class Path;
}

namespace az::tex::mathjax {
    namespace impl {
        struct GraphImpl;
    }

    struct AZTEX_EXPORT NodeProps {
        // label is upper case hex without 0x prefix
        const std::string
                id,
                svg_type,
                mml,
                label,
                text;
        TransformMatrix2 transform;
        Box2 view_box;
        ScalarType font_size, width, height, x, y, x1, y1, x2, y2;
        std::string path_id;
        bool data_frame;
        az::svg::Path *path = nullptr;
    };

    struct AZTEX_EXPORT NodePropsPtrHasher {
        std::size_t operator()(const NodeProps *k) const;
    };

    struct AZTEX_EXPORT EdgeProps {
    };

    class AZTEX_EXPORT Graph {
    public:
        explicit Graph();

        Graph(Graph &&g) noexcept;

        // unique_ptr under pimpl requires a visible destructor
        // https://stackoverflow.com/questions/9020372/how-do-i-use-unique-ptr-for-pimpl
        ~Graph();

        impl::GraphImpl *get_pimpl();

        void traverse(
                const std::function<void(const NodeProps &)> &on_discover_node,
                const std::function<void(const NodeProps &)> &on_finish_node);

        Box2 bbox(const NodeProps &node) const;

        Box2 bbox(const NodeProps &node, const TransformMatrix2 &mat) const;

    private:
        std::unique_ptr<impl::GraphImpl> p;
    };
}
