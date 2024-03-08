#pragma once

#include "az/pen/pen_stroke.h"
#include "az/pen/pen_char.h"
#include "az/data/inkml_item.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>


namespace az::data::impl {
    struct InkmlRawItem {
        std::string mml;
        std::string latex;
        std::unordered_map<std::string, size_t> mml_index_map;
        std::unordered_map<std::string, az::pen::PenStroke> id_path_map;
        std::vector<std::pair<
                std::string,
                std::vector<std::string>
        >> trace_groups;

        InkmlItem read(std::istream &in, const std::function<void(
                const az::pen::PenChar &,
                const LabelType &
        )> &on_char);
    };
}