#include "az/pen/pen_font.h"
#include "az/pen/pen_context.h"
#include "az/svg/path.h"
#include <spdlog/spdlog.h>

#include <utility>

using namespace az;
using namespace az::pen;

PenFont::PenFont(UCharType text, const Box2 &box, const TransformMatrix2 &transform)
        : text(std::move(text)), box(box) {
    this->trans = transform;
}

Box2 PenFont::bbox(const TransformMatrix2 &parent_trans) {
    Box2 new_box;
    Transform2 current_trans;
    current_trans.matrix() = parent_trans * this->trans;
    for (const auto &corner_enum: std::array<Box2::CornerType, 4>{
            Box2::TopLeft,
            Box2::TopLeft,
            Box2::BottomLeft,
            Box2::BottomRight,
    }) {
        new_box.extend(current_trans * this->box.corner(corner_enum));
    }
    return new_box;
}

void PenFont::on_paint(const TransformMatrix2 &parent_trans) {
    if (!this->context) {
        throw std::runtime_error("context is not registered in PenFont");
    }
    this->context->draw_text(this->text, this->box, parent_trans * this->trans);
}
