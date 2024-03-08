#include "az/core/geometry_def.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace az;

az::ScalarType az::get_distance(const Vec2 &p1, const Vec2 &p2) {
    return std::sqrt((p1 - p2).array().square().sum());
}

az::ScalarType az::get_distance(const Vec3 &p1, const Vec3 &p2) {
    return std::sqrt((p1 - p2).array().square().sum());
}

std::pair<az::ScalarType, az::ScalarType> az::get_box_size(const az::Box2 &box) {
    return {box.sizes().x(), box.sizes().y()};
}

std::pair<Vec2, Vec2> az::move_line_vertically(const Vec2 &p1, const Vec2 &p2, ScalarType offset) {
    // https://math.stackexchange.com/questions/2593627/i-have-a-line-i-want-to-move-the-line-a-certain-distance-away-parallelly
    auto r = get_distance(p1, p2);
    auto delta = p2 - p1;
    Vec2 offset_p{offset * -delta.y() / r, offset * delta.x() / r};
    return {p1 + offset_p, p2 + offset_p};
}

TransformMatrix2 az::get_affine_transform(const Vec2Array3 &src, const Vec2Array3 &dst) {
    TransformMatrix2 trans = TransformMatrix2::Identity();
    std::array<double, 6 * 6> a{};
    std::array<double, 6> b{};
    std::array<double, 6> c{};
    for (int i = 0; i < 3; i++) {
        int j = i * 12;
        int k = i * 12 + 6;
        a[j] = a[k + 3] = src[i].x();
        a[j + 1] = a[k + 4] = src[i].y();
        a[j + 2] = a[k + 5] = 1;
        a[j + 3] = a[j + 4] = a[j + 5] = 0;
        a[k] = a[k + 1] = a[k + 2] = 0;
        b[i * 2] = dst[i].x();
        b[i * 2 + 1] = dst[i].y();
    }
    // opencv is row major, above codes are from opencv/modules/imgproc/src/imgwarp.cpp
    Eigen::Map<Eigen::Matrix<double, 6, 6, Eigen::RowMajor>> A(a.data(), 6, 6);
    Eigen::Map<Eigen::Matrix<double, 6, 1>> B(b.data(), 6, 1);
    Eigen::Map<Eigen::Matrix<double, 6, 1>> C(c.data(), 6, 1);
    // https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
    // not work
//    C= A.jacobiSvd().solve(B);
//    C= A.bdcSvd().solve(B);
    // work but slow
//    C= A.fullPivLu().solve(B);
//    C= A.fullPivHouseholderQr().solve(B);
    C = A.colPivHouseholderQr().solve(B);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            trans(i, j) = c[i * 3 + j];
        }
    }
    return trans;
}
