#include "az/pen/pen_string.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace az;
using namespace az::pen;

Box2 PenString::bbox(const TransformMatrix2 &parent_trans) {
    Box2 new_box;
    const auto current_trans = parent_trans * this->trans;
    for (auto &item: this->data) {
        new_box.extend(item.bbox(current_trans));
    }
    return new_box;
}

void PenString::on_paint(const TransformMatrix2 &parent_trans) {
    if (!this->context) {
        throw std::runtime_error("context is not registered in PenString");
    }
    const auto current_trans = parent_trans * this->trans;
    for (auto &item: this->data) {
        item.set_context(this->context);
        item.on_paint(current_trans);
    }
}
