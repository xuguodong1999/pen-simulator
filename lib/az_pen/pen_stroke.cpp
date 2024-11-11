#include "az/pen/pen_stroke.h"
#include "az/pen/pen_context.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace az;
using namespace az::pen;

void PenStroke::apply_add(const Vec2 &offset) {
    this->points.colwise() += offset;
}

void PenStroke::apply_rotate(ScalarType radians, const Vec2 &center) {
    auto xs = this->points.row(0);
    auto ys = this->points.row(1);
    auto cos_rad = std::cos(radians);
    auto sin_rad = std::sin(radians);
    auto xs_ = xs.eval(); // deep copy to avoid data population
    this->points.row(0) = (xs.array() - center.x()) * cos_rad + (ys.array() - center.y()) * sin_rad + center.x();
    this->points.row(1) = (ys.array() - center.y()) * cos_rad - (xs_.array() - center.x()) * sin_rad + center.y();
}

void PenStroke::apply_dot(ScalarType kx, ScalarType ky) {
    this->points.col(0).array() *= kx;
    this->points.col(1).array() *= ky;
}

Box2 PenStroke::bbox(const TransformMatrix2 &parent_trans) {
    Box2 new_box;
    Transform2 current_trans;
    current_trans.matrix() = parent_trans * this->trans;
    for (const auto &p: this->points.colwise()) {
        new_box.extend(current_trans * p);
    }
    return new_box;
}

void PenStroke::set_data(SizeType size, const std::function<void(ScalarType &x, ScalarType &y)> &on_next) {
    this->points.resize(this->points.rows(), static_cast<Eigen::Index>(size));
    for (auto p: this->points.colwise()) {
        on_next(p.x(), p.y());
    }
}

void PenStroke::on_paint(const TransformMatrix2 &parent_trans) {
    if (!this->context) {
        throw std::runtime_error("context is not registered in PenStroke");
    }
    Transform2 current_trans;
    current_trans.matrix() = parent_trans * this->trans;
    if (this->points.cols() == 0) {
        return;
    } else if (this->points.cols() == 1) {
        Vec2 p = current_trans * this->points.col(0);
        context->draw_line(p.x(), p.y(), p.x(), p.y());
        return;
    }
    Vec2 p0 = current_trans * this->points.col(0); // i-1
    for (int i = 1; i < this->points.cols(); i++) {
        Vec2 p1 = current_trans * this->points.col(i);
        this->context->draw_line(p0.x(), p0.y(), p1.x(), p1.y());
        p0 = p1;
    }
}

Vec2Array PenStroke::as_points() {
    Vec2Array pts;
    pts.reserve(this->count());
    Transform2 current_trans;
    current_trans.matrix() = this->trans;
    for (auto p: this->points.colwise()) {
        pts.push_back(current_trans * p);
    }
    return pts;
}

size_t PenStroke::count() const {
    return this->points.cols();
}
