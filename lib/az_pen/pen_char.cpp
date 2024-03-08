#include "az/pen/pen_char.h"
#include "az/pen/pen_stroke.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace az;
using namespace az::pen;

Box2 PenChar::bbox(const TransformMatrix2 &parent_trans) {
    Box2 new_box;
    const auto current_trans = parent_trans * this->trans;
    for (auto &item: this->data) {
        new_box.extend(item.bbox(current_trans));
    }
    return new_box;
}

void PenChar::on_paint(const TransformMatrix2 &parent_trans) {
    if (!this->context) {
        throw std::runtime_error("context is not registered in PenChar");
    }
    const auto current_trans = parent_trans * this->trans;
    for (auto &item: this->data) {
        item.set_context(this->context);
        item.on_paint(current_trans);
    }
}

Vec2Array PenChar::as_points() {
    Vec2Array pts;
    size_t count = 0;
    for (const PenStroke &item: this->data) { count += item.count(); }
    pts.reserve(count);

    Transform2 current_trans;
    current_trans.matrix() = this->trans;
    for (auto &item: this->data) {
        auto sub_pts = item.as_points();
        for (auto &pt: sub_pts) {
            pts.push_back(current_trans * pt);
        }
    }
    return pts;
}
