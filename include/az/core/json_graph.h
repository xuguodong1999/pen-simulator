#pragma once

#include <vector>
#include <string>
#include <unordered_map>

/**
 * https://github.com/jsongraph/json-graph-specification
 */
namespace az {
    template<typename Meta>
    struct JsonGraphNode {
        std::string label;
        Meta metadata;
    };

    template<typename Meta>
    struct JsonGraphEdge {
        std::string source, target, relation, label;
        bool directed;
        Meta metadata;
    };

    template<typename Node, typename Edge, typename Meta>
    struct JsonGraph {
        std::string label, type;
        bool directed;
        Meta metadata;
        std::unordered_map<std::string, Node> nodes;
        std::vector<Edge> edges;
    };
}
