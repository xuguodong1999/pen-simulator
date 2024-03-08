#include "az/pen/pen_path.h"
#include "az/pen/pen_context.h"
#include "az/svg/path.h"

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;

PenPath::PenPath(std::string_view d) : PenOp() {
    this->path = std::make_shared<az::svg::Path>(d);
    this->dot(1, -1);
}

PenPath::PenPath(az::svg::Path *path, const TransformMatrix2 &trans)
        : path(std::make_shared<az::svg::Path>(*path)) {
    this->trans = trans;
}

PenPath::PenPath(const PenPath &other) {
    *this = other;
}

PenPath &PenPath::operator=(const PenPath &other) {
    if (this == &other) { return *this; }
    this->trans = other.trans;
    this->bounding_box = other.bounding_box;
    this->context = other.context;
    this->path = std::make_shared<az::svg::Path>(*(other.path));
    return *this;
}

Box2 PenPath::bbox(const TransformMatrix2 &parent_trans) {
    return this->path->bbox(parent_trans * this->trans);
}

void PenPath::on_paint(const TransformMatrix2 &parent_trans) {
    if (!this->context) {
        throw std::runtime_error("context is not registered in PenPath");
    }
    this->context->draw_path(this->path.get(), parent_trans * this->trans);
}
