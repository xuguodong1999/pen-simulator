#include "az/pen/pen_op.h"
#include "az/pen/pen_context.h"
#include <boost/math/constants/constants.hpp>

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;

void PenOp::invalidate_box() {
    this->bounding_box.setEmpty();
}

void PenOp::add(const Vec2 &offset) {
    Transform2 transform{
            Eigen::Translation<ScalarType, 2>{offset.x(), offset.y()}
    };
    this->apply_transform(transform.matrix());
    invalidate_box();
}

void PenOp::rotate(ScalarType radians, const Vec2 &center) {
    this->add(-center);
    Transform2 transform{
            Eigen::Rotation2D<ScalarType>{radians}
    };
    this->apply_transform(transform.matrix());
    this->add(center);
    invalidate_box();
}

void PenOp::dot(ScalarType kx, ScalarType ky) {
    Transform2 transform{
            Eigen::Scaling(kx, ky)
    };
    this->apply_transform(transform.matrix());
    invalidate_box();
}

void PenOp::apply_transform(const TransformMatrix2 &matrix) {
    this->trans = matrix * this->trans;
}

const TransformMatrix2 &PenOp::get_transform() const {
    return this->trans;
}

void PenOp::dot(ScalarType k) {
    this->dot(k, k);
}

void PenOp::move_tl_to(const Vec2 &dst) {
    const Box2 box = this->bbox();
    this->add(dst - box.min());
}

void PenOp::move_center_to(const Vec2 &dst) {
    const Box2 box = this->bbox();
    this->add(dst - box.center());
}

void PenOp::fit_into(ScalarType w, ScalarType h) {
    const Box2 box = this->bbox();
    Vec2 size = box.max() - box.min();
    if (size.x() < 0.001) { size.x() = 0.001; }
    if (size.y() < 0.001) { size.y() = 0.001; }
    ScalarType kw = 1, kh = 1;
    if (size.x() > 0) {
        kw = w / size.x();
    }
    if (size.y() > 0) {
        kh = h / size.y();
    }
    this->move_tl_to({0, 0});
    this->dot(kw, kh);
}

void PenOp::fit_into_keep_ratio(ScalarType w, ScalarType h) {
    const Box2 box = this->bbox();
    Vec2 size = box.sizes();
    ScalarType kw = 1, kh = 1;
    if (size.x() > 0) {
        kw = w / size.x();
    }
    if (size.y() > 0) {
        kh = h / size.y();
    }
    this->add(-box.min());
    ScalarType k = (std::min)(kw, kh);
    this->dot(k);
    this->add({(w - size.x() * k) / 2, (h - size.y() * k) / 2});
}

void PenOp::rotate(ScalarType radians) {
    this->rotate(radians, this->bbox().center());
}

void PenOp::rotate_deg(ScalarType degree) {
    this->rotate(boost::math::constants::pi<ScalarType>() / 180 * degree);
}

Box2 PenOp::bbox() {
    if (!this->bounding_box.isEmpty()) { return this->bounding_box; }
    this->bounding_box = this->bbox(TransformMatrix2::Identity());
    return this->bounding_box;
}

void PenOp::on_paint() {
    this->on_paint(TransformMatrix2::Identity());
}

void PenOp::set_context(std::shared_ptr<PenContext> c) {
    this->context = std::move(c);
}

Vec2Array PenOp::as_points() {
    auto rect = this->bbox();
    return {
            rect.corner(Box2::TopLeft),
            rect.corner(Box2::TopRight),
            rect.corner(Box2::BottomLeft),
            rect.corner(Box2::BottomRight),
    };
}