#include "transform_events_policy.h"
#include <boost/math/constants/constants.hpp>

using namespace az::svg::impl::svgpp;

void TransformEventsPolicy::transform_matrix(TransformMatrix2 &transform, const boost::array<double, 6> &matrix) {
    TransformMatrix2 m(3, 3);
    m << matrix[0], matrix[2], matrix[4],
            matrix[1], matrix[3], matrix[5],
            0, 0, 1;
    transform *= m;
}

void TransformEventsPolicy::transform_translate(TransformMatrix2 &transform, double tx, double ty) {
    TransformMatrix2 m = TransformMatrix2::Identity();
    m(0, 2) = tx;
    m(1, 2) = ty;
    transform *= m;
}

void TransformEventsPolicy::transform_translate(TransformMatrix2 &transform, double tx) {
    transform_translate(transform, tx, 0);
}

void TransformEventsPolicy::transform_scale(TransformMatrix2 &transform, double sx, double sy) {
    TransformMatrix2 m = TransformMatrix2::Identity();
    m(0, 0) = sx;
    m(1, 1) = sy;
    transform *= m;
}

void TransformEventsPolicy::transform_scale(TransformMatrix2 &transform, double scale) {
    transform_scale(transform, scale, scale);
}

void TransformEventsPolicy::transform_rotate(TransformMatrix2 &transform, double angle) {
    angle *= boost::math::constants::degree<double>();
    TransformMatrix2 m(3, 3);
    double cos_a = std::cos(angle);
    double sin_a = std::sin(angle);
    m << cos_a, -sin_a, 0,
            sin_a, cos_a, 0,
            0, 0, 1;
    transform *= m;
}

void TransformEventsPolicy::transform_rotate(TransformMatrix2 &transform, double angle, double cx, double cy) {
    angle *= boost::math::constants::degree<double>();
    TransformMatrix2 m(3, 3);
    double cos_a = std::cos(angle);
    double sin_a = std::sin(angle);
    m << cos_a, -sin_a, -cx * cos_a + cy * sin_a + cx,
            sin_a, cos_a, -cx * sin_a - cy * cos_a + cy,
            0, 0, 1;
    transform *= m;
}

void TransformEventsPolicy::transform_skew_x(TransformMatrix2 &transform, double angle) {
    angle *= boost::math::constants::degree<double>();
    TransformMatrix2 m = TransformMatrix2::Identity();
    m(0, 1) = std::tan(angle);
    transform *= m;
}

void TransformEventsPolicy::transform_skew_y(TransformMatrix2 &transform, double angle) {
    angle *= boost::math::constants::degree<double>();
    TransformMatrix2 m = TransformMatrix2::Identity();
    m(1, 0) = std::tan(angle);
    transform *= m;
}
