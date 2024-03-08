#include "az/svg/path.h"

#include <SkParsePath.h>

#include <SkMatrix.h>
#include <SkRect.h>
#include <SkPath.h>
#include <SkCanvas.h>

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::svg;

static SkMatrix convert_eigen_33_to_sk_matrix(const TransformMatrix2 &mat) {
    return SkMatrix::MakeAll(
            mat(0, 0), mat(0, 1), mat(0, 2),
            mat(1, 0), mat(1, 1), mat(1, 2),
            mat(2, 0), mat(2, 1), mat(2, 2));
}

static Box2 convert_sk_rect_to_eigen_2(const SkRect &sk_rect) {
    Box2 box;
    box.extend(Vec2{sk_rect.left(), sk_rect.top()});
    box.extend(Vec2{sk_rect.right(), sk_rect.bottom()});
    return box;
}

Path::Path(std::string_view in) {
    sk_path = std::make_shared<SkPath>();
    bool success = SkParsePath::FromSVGString(in.data(), sk_path.get());
    if (!success) {
        sk_path = nullptr;
        SPDLOG_ERROR("SkParsePath::FromSVGString failed: {}", in);
    } else {
        d_path = in;
    }
}

Path::Path(const Path &other) {
    *this = other;
}

Path &Path::operator=(const Path &other) {
    if (this == &other) { return *this; }
    d_path = other.d_path;
    sk_path = std::make_shared<SkPath>(*(other.sk_path));
//    svgpp_path = other.svgpp_path;
    return *this;
}

Box2 Path::bbox() {
    if (!sk_path) {
        return Box2{};
    }
    auto sk_rect = sk_path->computeTightBounds();
    return convert_sk_rect_to_eigen_2(sk_rect);
}

Box2 Path::bbox(const TransformMatrix2 &mat) const {
    if (!sk_path) {
        return Box2{};
    }
    SkMatrix sk_mat = convert_eigen_33_to_sk_matrix(mat);
    SkPath path = sk_path->makeTransform(sk_mat);
    auto sk_rect = path.computeTightBounds();
    return convert_sk_rect_to_eigen_2(sk_rect);
}

SkPath *Path::get_path() const {
    return sk_path.get();
}

const std::string &Path::get_d_path() const {
    return d_path;
}