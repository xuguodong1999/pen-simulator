#pragma once

#include "az/core/config.h"

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/Geometry>

namespace az {
    using ScalarType = float;
    using Vec2 = Eigen::Vector2<ScalarType>;
    using Vec3 = Eigen::Vector3<ScalarType>;
    using Mat2 = Eigen::Matrix2X<ScalarType>;
    // bl, tr
    using Box2 = Eigen::AlignedBox<ScalarType, 2>;
    using TransformMatrix2 = Eigen::Matrix<ScalarType, 3, 3>;
    using Transform2 = Eigen::Transform<ScalarType, 2, Eigen::Affine>;
    using Vec2Array = std::vector<Vec2, Eigen::aligned_allocator<Vec2> >;
    using Vec2Array3 = std::array<Vec2, 3>;

    az::ScalarType AZCORE_EXPORT get_distance(const Vec2 &p1, const Vec2 &p2);

    az::ScalarType AZCORE_EXPORT get_distance(const Vec3 &p1, const Vec3 &p2);

    std::pair<az::ScalarType, az::ScalarType> AZCORE_EXPORT get_box_size(const az::Box2 &box);

    std::pair<Vec2, Vec2> AZCORE_EXPORT move_line_vertically(const Vec2 &p1, const Vec2 &p2, ScalarType offset);

    TransformMatrix2 AZCORE_EXPORT get_affine_transform(const Vec2Array3 &src, const Vec2Array3 &dst);
}

