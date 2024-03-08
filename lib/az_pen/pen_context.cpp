#include "az/pen/pen_context.h"
#include "az/svg/path.h"

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;

//#define XGD_DEBUG

void PenContext::register_draw_line(const PenContext::fn_draw_line_type &fn) {
    this->fn_draw_line = fn;
}

void PenContext::register_draw_path(const PenContext::fn_draw_path_type &fn) {
    this->fn_draw_path = fn;
}

void PenContext::register_draw_skpath(const PenContext::fn_draw_skpath_type &fn) {
    this->fn_draw_skpath = fn;
}

void PenContext::register_draw_text(const PenContext::fn_draw_text_type &fn) {
    this->fn_draw_text = fn;
}

void PenContext::register_on_label(const PenContext::fn_on_label_type &fn) {
    this->fn_on_label = fn;
}

void PenContext::register_on_hierarchy(const PenContext::fn_on_hierarchy_type &fn) {
    this->fn_on_hierarchy = fn;
}

void PenContext::on_label(const LabelType &origin, const LabelType &fallback) {
    if (!this->fn_on_label) {
        return; // this is optional
    }
    this->fn_on_label(origin, fallback);
}

void PenContext::on_hierarchy(const std::vector<LabelType> &hierarchy, const LabelType &label) {
    if (!this->fn_on_hierarchy) {
        return; // this is optional
    }
    this->fn_on_hierarchy(hierarchy, label);
}

void PenContext::draw_line(const ScalarType &x0, const ScalarType &y0, const ScalarType &x1, const ScalarType &y1) {
    if (!this->fn_draw_line) {
        throw std::runtime_error("draw_line not registered in PenContext");
    }
#ifdef XGD_DEBUG
    SPDLOG_INFO("draw_line: ({}, {}), ({}, {})",x0, y0, x1, y1);
#endif
    this->fn_draw_line(x0, y0, x1, y1);
}

void PenContext::draw_path(const az::svg::Path *p0, const az::TransformMatrix2 &p1) {
    if (!this->fn_draw_path && !this->fn_draw_skpath) {
        throw std::runtime_error("draw_path or draw_skpath not registered in PenContext");
    }
    if (this->fn_draw_path) {
        this->fn_draw_path(p0->get_d_path(), p1);
    }
    if (this->fn_draw_skpath) {
        this->fn_draw_skpath(p0->get_path(), p1);
    }
}

void PenContext::draw_text(const az::UCharType &p0, const az::Box2 &p1, const az::TransformMatrix2 &p2) {
    if (!this->fn_draw_text) {
        throw std::runtime_error("draw_text not registered in PenContext");
    }
    this->fn_draw_text(p0, p1, p2);
}
