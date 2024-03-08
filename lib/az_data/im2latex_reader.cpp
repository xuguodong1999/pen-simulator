#include "az/data/im2latex_reader.h"
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

using namespace az;
using namespace az::data;
using namespace az::pen;

void Im2LatexReader::sync_load_all(std::string_view file_path) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(file_path.data(), pt);
    auto &valid_root = pt.get_child("valid");
    auto &invalid_root = pt.get_child("invalid");
    data_source.reserve(valid_root.size() + invalid_root.size());
    label_source.reserve(valid_root.size() + invalid_root.size());
    for (auto &[k, v]: valid_root) {
        data_source.push_back(v.data());
    }
    label_source.resize(valid_root.size(), 1);
    for (auto &[k, v]: invalid_root) {
        data_source.push_back(v.data());
    }
    label_source.resize(data_source.size(), 0);
}

std::pair<LabelType *, int8_t *> Im2LatexReader::next() {
    static size_t i = 0;
    if (i == data_source.size()) { i = 0; }
    auto &item = data_source[i];
    auto &label = label_source[i++];
    return {&item, &label};
}

SizeType Im2LatexReader::size() const {
    return data_source.size();
}

void Im2LatexReader::binary_serialize(std::ostream &out) {
}

void Im2LatexReader::binary_deserialize(std::istream &in) {
}
