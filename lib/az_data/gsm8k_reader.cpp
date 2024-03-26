#include "az/data/gsm8k_reader.h"

using namespace az;
using namespace az::data;

void Gsm8kReader::sync_load_all(std::string_view root_dir) {
}

std::pair<az::pen::PenGraph *, LabelType *> Gsm8kReader::next() {
    return {};
}

az::SizeType Gsm8kReader::size() const {
    return 0;
}

void Gsm8kReader::binary_serialize(std::ostream &out) {
}

void Gsm8kReader::binary_deserialize(std::istream &in) {
}
