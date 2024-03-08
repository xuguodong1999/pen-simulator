#include "az/core/geometry_def.h"

#include <opencv2/imgproc.hpp>

#include <boost/math/constants/constants.hpp>

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <string>

using namespace az;

static TransformMatrix2 opencv_get_affine_transform(const Vec2Array3 &src, const Vec2Array3 &dst) {
    TransformMatrix2 trans;
    trans.setIdentity();

    std::vector<cv::Point2f> src_pts;
    src_pts.reserve(src.size());
    for (auto &pt: src) {
        src_pts.emplace_back(pt.x(), pt.y());
    }

    std::vector<cv::Point2f> dst_pts;
    dst_pts.reserve(dst.size());
    for (auto &pt: dst) {
        dst_pts.emplace_back(pt.x(), pt.y());
    }
    auto matrix = cv::getAffineTransform(src_pts.data(), dst_pts.data());
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            trans(i, j) = matrix.at<double>(i, j);
        }
    }
    return trans;
}

static Vec2Array get_min_area_rect(const std::vector<Vec2> &pts) {
    std::vector<cv::Point2f> container;
    container.reserve(pts.size());
    for (auto &pt: pts) {
        container.emplace_back(pt.x(), pt.y());
    }
    auto rr = cv::minAreaRect(container);
    SPDLOG_INFO(rr.center);
    SPDLOG_INFO(rr.size);
    Transform2 transform{
            Eigen::Rotation2D<ScalarType>{boost::math::constants::pi<ScalarType>() / 180 * rr.angle}
    };
    Box2 aligned_box{
            Vec2{rr.center.x - rr.size.width / 2, rr.center.y - rr.size.height / 2,},
            Vec2{rr.center.x + rr.size.width / 2, rr.center.y + rr.size.height / 2,}
    };
    const ScalarType LINE_WIDTH_RATIO = 0.01;
    auto w = aligned_box.sizes().x();
    auto h = aligned_box.sizes().y();
    bool case1 = w < h && w < LINE_WIDTH_RATIO * h;
    bool case2 = h < w && h < LINE_WIDTH_RATIO * w;
//    if (false) {
    if (case1 || case2) {
        auto offset = LINE_WIDTH_RATIO * (case1 ? h : w);
        auto [p0, p1] = move_line_vertically(
                aligned_box.min(),
                aligned_box.max(),
                offset
        );
        auto [p2, p3] = move_line_vertically(
                aligned_box.min(),
                aligned_box.max(),
                -offset
        );
        aligned_box.extend(p0);
        aligned_box.extend(p1);
        aligned_box.extend(p2);
        aligned_box.extend(p3);
    }
    return {
            transform * aligned_box.corner(Box2::TopLeft),
            transform * aligned_box.corner(Box2::TopRight),
            transform * aligned_box.corner(Box2::BottomLeft),
            transform * aligned_box.corner(Box2::BottomRight),
    };
}

#define EXPECT_EQ_AT_PRECISION_1(a, b) EXPECT_EQ(fmt::format("{:.1f}", a), fmt::format("{:.1f}", b))
#define EXPECT_EQ_AT_PRECISION_2(a, b) EXPECT_EQ(fmt::format("{:.2f}", a), fmt::format("{:.2f}", b))
#define EXPECT_EQ_AT_PRECISION_3(a, b) EXPECT_EQ(fmt::format("{:.3f}", a), fmt::format("{:.3f}", b))
#define EXPECT_EQ_AT_PRECISION_4(a, b) EXPECT_EQ(fmt::format("{:.4f}", a), fmt::format("{:.4f}", b))
#define EXPECT_EQ_AT_PRECISION_5(a, b) EXPECT_EQ(fmt::format("{:.5f}", a), fmt::format("{:.5f}", b))
#define EXPECT_EQ_AT_PRECISION_6(a, b) EXPECT_EQ(fmt::format("{:.6f}", a), fmt::format("{:.6f}", b))

TEST(test_core, eigen_opencv_affine) {
    Vec2Array3 src{
            Vec2{1, 1},
            Vec2{1, 3},
            Vec2{2, 1},
//            Vec2{2, 3},
    };
    Vec2Array3 dst{
            Vec2{3, 3},
            Vec2{5, 3},
            Vec2{3, 4},
//            Vec2{5, 4},
    };
    auto mat1 = az::get_affine_transform(src, dst);
    auto mat2 = opencv_get_affine_transform(src, dst);
    SPDLOG_INFO("mat1={}", mat1);
    SPDLOG_INFO("mat2={}", mat2);
    for (size_t i = 0; i < 9; i++) {
        EXPECT_EQ_AT_PRECISION_6(mat1.data()[i], mat2.data()[i]);
    }
}

TEST(eigen_opencv, min_area_box) {
}
