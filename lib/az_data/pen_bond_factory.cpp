#include "az/data/pen_bond_factory.h"
#include "az/data/synthesis_tex_generator.h"

#include "az/pen/pen_path.h"

#include <opencv2/imgproc.hpp>

#include <boost/math/constants/constants.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace az;
using namespace az::data;
using namespace az::pen;

// TODO: rewrite convex hull using Eigen and remove opencv dependencies
static Vec2Array3 cv_get_min_area_rect(const Vec2Array &pts) {
    std::vector<cv::Point2f> container;
    container.reserve(pts.size());
    for (auto &pt: pts) {
        container.emplace_back(pt.x(), pt.y());
    }
    std::array<cv::Point2f, 4> rect_corners;
    cv::RotatedRect rr = cv::minAreaRect(container);
    rr.points(rect_corners.data());
    Vec2 p0{rect_corners[0].x, rect_corners[0].y};
    Vec2 p1{rect_corners[1].x, rect_corners[1].y};
    Vec2 p2{rect_corners[2].x, rect_corners[2].y};
    Vec2 p3{rect_corners[3].x, rect_corners[3].y};
    if (get_distance(p0, p1) > get_distance(p0, p3)) {
        return {(p0 + p3) / 2, p1, p2};
    } else {
        return {(p0 + p1) / 2, p3, p2};
    }
}

static std::shared_ptr<PenOp> copy_line_shape(const PenOp *raw_shape) {
    std::shared_ptr<PenOp> item;
    if (auto pen_char = dynamic_cast<const PenChar *>(raw_shape)) {
        item = std::make_shared<PenChar>(*pen_char);
    } else if (auto pen_path = dynamic_cast<const PenPath *>(raw_shape)) {
        item = std::make_shared<PenPath>(*pen_path);
    } else if (auto pen_graph = dynamic_cast<const PenGraph *>(raw_shape)) {
        item = std::make_shared<PenGraph>(*pen_graph);
    } else {
        item = SynthesisTexGenerator::generate_sub_string("-");
    }
    return item;
}

PenBondFactory::Output PenBondFactory::make_line_bond(
        const ShapeProvider &shape_provider,
        const az::Vec2 &from,
        const az::Vec2 &to,
        int8_t order
) {
//    SPDLOG_INFO("make line bond order={}", order);
    if (order <= 0) { throw std::runtime_error("order==0"); }
    auto graph = std::make_shared<PenGraph>();
    graph->data.reserve(order);
    const ScalarType LINE_WIDTH_RATIO = 0.1;
    const ScalarType LINE_DISTANCE_RATIO = 0.05;
    // order: 1 2 3 4
    // index: 0 1 2 3
    const bool skip_zero = ((order - 1) % 2) == 1;
    const int upper_bound = std::ceil((ScalarType(order) - 1) / 2);
    const auto distance = get_distance(from, to);
    const auto offset = distance * LINE_WIDTH_RATIO / 2.f;
    const auto inc = distance * (LINE_WIDTH_RATIO + LINE_DISTANCE_RATIO);
    Box2 bbox;
    bbox.extend(from);
    bbox.extend(to);
    for (int i = -upper_bound; i <= upper_bound; i++) {
        if (i == 0 && skip_zero) { continue; }
        auto raw_shape = shape_provider("-");
        auto item = copy_line_shape(raw_shape);
        auto src_pts = cv_get_min_area_rect(item->as_points());
        const auto current_offset = inc * ScalarType(i);
        auto [p0, p1] = move_line_vertically(from, to, current_offset + offset);
        auto [p2, p3] = move_line_vertically(from, to, current_offset - offset);
        Vec2Array3 dst_pts{(p0 + p2) / 2, p1, p3};
        item->apply_transform(az::get_affine_transform(src_pts, dst_pts));
        graph->data.push_back(item);
    }
    return graph;
}

PenBondFactory::Output PenBondFactory::make_solid_wedge_bond(
        const ShapeProvider &shape_provider,
        const az::Vec2 &from,
        const az::Vec2 &to
) {
    const size_t LINE_COUNT = 5;
    const ScalarType LINE_WIDTH_RATIO = 0.3;
    const auto distance = get_distance(from, to);
    const auto offset = distance * LINE_WIDTH_RATIO / 2.f;
    auto [p0_, p1_] = move_line_vertically(from, to, offset);
    auto [p2_, p3_] = move_line_vertically(from, to, -offset);
    // from, p1
    // from, p3
    auto graph = std::make_shared<PenGraph>();
    graph->data.reserve(LINE_COUNT);
    for (size_t i = 0; i <= LINE_COUNT; i++) {
        auto to_ = p1_ + (p3_ - p1_) * (ScalarType(i) / LINE_COUNT);
        auto raw_shape = shape_provider("-");
        auto item = copy_line_shape(raw_shape);
        auto src_pts = cv_get_min_area_rect(item->as_points());
        const auto distance_ = get_distance(from, to_);
        const auto offset_ = distance_ * ScalarType{0.01};
        auto [p0, p1] = move_line_vertically(from, to_, offset_);
        auto [p2, p3] = move_line_vertically(from, to_, -offset_);
        Vec2Array3 dst_pts{from, p1, p3};
        item->apply_transform(get_affine_transform(src_pts, dst_pts));
        graph->data.push_back(item);
    }
    return graph;
}

PenBondFactory::Output PenBondFactory::make_dash_wedge_bond(
        const ShapeProvider &shape_provider,
        const az::Vec2 &from,
        const az::Vec2 &to
) {
    const size_t LINE_COUNT = 5;
    const ScalarType LINE_WIDTH_RATIO = 0.3;
    const auto distance = get_distance(from, to);
    const auto offset = distance * LINE_WIDTH_RATIO / 2.f;
    auto [p0_, p1_] = move_line_vertically(from, to, offset);
    auto [p2_, p3_] = move_line_vertically(from, to, -offset);
    // from, p1
    // from, p3
    auto graph = std::make_shared<PenGraph>();
    graph->data.reserve(LINE_COUNT);
    for (size_t i = 1; i <= LINE_COUNT; i++) {
        Vec2 from_ = from + (p1_ - from) * (ScalarType(i) / LINE_COUNT);
        Vec2 to_ = from + (p3_ - from) * (ScalarType(i) / LINE_COUNT);
        auto raw_shape = shape_provider("-");
        auto item = copy_line_shape(raw_shape);
        auto src_pts = cv_get_min_area_rect(item->as_points());
        const auto distance_ = get_distance(from_, to_);
        const auto offset_ = distance_ * ScalarType{0.1};
        auto [p0, p1] = move_line_vertically(from_, to_, offset_);
        auto [p2, p3] = move_line_vertically(from_, to_, -offset_);
        Vec2Array3 dst_pts{from_, p1, p3};
        item->apply_transform(get_affine_transform(src_pts, dst_pts));
        graph->data.push_back(item);
    }
    return graph;
}

PenBondFactory::Output PenBondFactory::make_wave_bond(
        const ShapeProvider &shape_provider,
        const az::Vec2 &from,
        const az::Vec2 &to) {
    // TODO: impl it like COCR
    return make_line_bond(shape_provider, from, to, 1);
}

PenBondFactory::Output PenBondFactory::make_circle_bond(
        const ShapeProvider &shape_provider,
        const az::Box2 &bounding_box) {
    auto graph = std::make_shared<PenGraph>();
    auto raw_shape = shape_provider("o");
    std::shared_ptr<PenOp> item;
    if (auto pen_char = dynamic_cast<PenChar *>(raw_shape)) {
        item = std::make_shared<PenChar>(*pen_char);
    } else if (auto pen_path = dynamic_cast<PenPath *>(raw_shape)) {
        item = std::make_shared<PenPath>(*pen_path);
    } else if (auto pen_graph = dynamic_cast<PenGraph *>(raw_shape)) {
        item = std::make_shared<PenGraph>(*pen_graph);
    } else {
        item = SynthesisTexGenerator::generate_sub_string("o");
    }
    item->fit_into_keep_ratio(bounding_box.sizes().x(), bounding_box.sizes().y());
    graph->data.reserve(1);
    graph->data.push_back(item);
    return graph;
}
