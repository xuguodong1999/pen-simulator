#include "az/data/makemeahanzi_reader.h"
#include "az/pen/pen_path.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <boost/property_tree/json_parser.hpp>

using namespace az;
using namespace az::pen;
using namespace az::data;

void UnicodeSvgReader::sync_load_all(std::string_view graphics_txt_path) {
    using namespace boost;
    std::ifstream in(graphics_txt_path.data());
    if (!in.is_open()) {
        throw std::runtime_error(fmt::format("failed to open {}", graphics_txt_path));
    }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ism(line);
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ism, pt);
        auto &character = pt.get_child("character");
        auto &strokes = pt.get_child("strokes");
        auto sample = std::make_shared<PenGraph>();
        sample->data.reserve(strokes.size());
        for (auto &[k, v]: strokes) {
            auto path = std::make_shared<PenPath>(v.data());
            sample->data.push_back(path);
        }
        auto &label = character.data();
        insert_unordered_map_vector(data_source, label, sample);
    }
    keys.reserve(data_source.size());
    for (auto &[k, v]: data_source) {
        keys.push_back(k);
    }
}

void UnicodeSvgReader::binary_serialize(std::ostream &out) {
}

void UnicodeSvgReader::binary_deserialize(std::istream &in) {
}

PenGraph *UnicodeSvgReader::select(const UCharType &label) {
    PointerVec<PenGraph> *bucket = nullptr;
    auto it = data_source.find(label);
    if (it != data_source.end()) {
        bucket = &(it->second);
    }
    if (!bucket) { return nullptr; }
    if (bucket->empty()) {
        throw std::runtime_error(fmt::format("select: empty bucket for label \"{}\"", label));
    }
    auto &item = random_select(*bucket);
    return item.get();
}

std::pair<PenGraph *, UCharType *> UnicodeSvgReader::next() {
    if (keys.empty()) {
        throw std::runtime_error("next: empty keys");
    }
    auto &random_key = random_select(keys);
    PointerVec<PenGraph> &bucket = data_source[random_key];
    if (bucket.empty()) {
        throw std::runtime_error("next: empty bucket");
    }
    auto &item = random_select(bucket);
    return {item.get(), &random_key};
}

SizeType UnicodeSvgReader::size() const {
    SizeType size = 0;
    for (auto &[label, bucket]: data_source) {
        size += bucket.size();
    }
    return size;
}

const std::vector<UCharType> &UnicodeSvgReader::get_keys() const {
    return keys;
}
