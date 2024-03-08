#pragma once

#include "az/chem/mol_graph.h"

#include <boost/graph/adjacency_list.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>

namespace az::chem::impl {
    using namespace az::chem;

    struct GraphProps {
    };

    struct GraphImpl : boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            Atom,
            Bond,
            GraphProps,
            boost::listS> {
        void bfs_traverse(
                const std::function<void(const Atom &)> &on_discover_node,
                const std::function<void(const Bond &)> &on_edge) const;

        void dfs_traverse(
                const std::function<void(const Atom &)> &on_discover_node,
                const std::function<void(const Bond &)> &on_edge) const;
    };
}