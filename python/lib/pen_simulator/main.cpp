#include "az/pen/pen_op.h"
#include "az/pen/pen_context.h"
#include "az/data/structural_formula_item.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/data/couch_reader.h"
#include "az/data/makemeahanzi_reader.h"


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace py = pybind11;

enum class SourceType : int32_t {
    HANDWRITING = 0,
    SVG = 1
};


enum class TextType : int32_t {
    LATEX = 0,
    SMILES = 1
};

//#define XGD_DEBUG

PYBIND11_MODULE(pen_simulator, m) {
    using namespace az;
    using namespace az::pen;
    using namespace az::data;
    py::enum_<SynthesisTexGenerator::TraverseOrder>(m, "TraverseOrder")
            .value("DEFAULT", SynthesisTexGenerator::TraverseOrder::DEFAULT)
            .value("SORT_BY_MULTIPLY_DEMONSTRATION",
                   SynthesisTexGenerator::TraverseOrder::SORT_BY_MULTIPLY_DEMONSTRATION)
            .export_values();

    py::enum_<SourceType>(m, "SourceType")
            .value("HANDWRITING", SourceType::HANDWRITING)
            .value("SVG", SourceType::SVG)
            .export_values();

    py::enum_<TextType>(m, "TextType")
            .value("LATEX", TextType::LATEX)
            .value("SMILES", TextType::SMILES)
            .export_values();
    // convert AlignedBox's tl and br to Matrix
    using PyBox2 = Eigen::Matrix<ScalarType, 2, 2>;
    using py_fn_draw_text_type = std::function<void(
            const UCharType &,
            const PyBox2 &,
            const TransformMatrix2 &
    )>;
    py::class_<PenContext, std::shared_ptr<PenContext>>(m, "PenContext")
            .def(py::init<>())
            .def("register_draw_line", &PenContext::register_draw_line)
            .def("register_draw_path", &PenContext::register_draw_path)
            .def("register_draw_text", [](PenContext &self, const py_fn_draw_text_type &callback) {
                // passing & here leads to SIGSEGV on gcc-11, TODO: figure out why
                self.register_draw_text([=](const UCharType &a, const Box2 &b, const TransformMatrix2 &c) {
                    if (!callback) { return; }
                    PyBox2 py_box;
                    py_box.col(0) = b.min();
                    py_box.col(1) = b.max();
                    callback(a, py_box, c);
                });
            })
            .def("register_on_label", &PenContext::register_on_label)
            .def("register_on_hierarchy", &PenContext::register_on_hierarchy);

    py::class_<PenOp, std::shared_ptr<PenOp>>(m, "PenOp")
            .def("add", &PenOp::add)
            .def("bbox", py::overload_cast<>(&PenOp::bbox))
            .def("bbox", py::overload_cast<const TransformMatrix2 &>(&PenOp::bbox))
            .def("dot", py::overload_cast<ScalarType, ScalarType>(&PenOp::dot))
            .def("dot", py::overload_cast<ScalarType>(&PenOp::dot))
            .def("fit_into_keep_ratio", &PenOp::fit_into_keep_ratio)
            .def("fit_into", &PenOp::fit_into)
            .def("move_center_to", &PenOp::move_center_to)
            .def("on_paint", py::overload_cast<>(&PenOp::on_paint))
            .def("on_paint", py::overload_cast<const TransformMatrix2 &>(&PenOp::on_paint))
            .def("rotate_deg", &PenOp::rotate_deg)
            .def("rotate", py::overload_cast<ScalarType, const Vec2 &>(&PenOp::rotate))
            .def("rotate", py::overload_cast<ScalarType>(&PenOp::rotate))
            .def("set_context", &PenOp::set_context);

    static CouchDatasetReader hw_reader;
    static std::once_flag couch_flag;
    m.def("global_init_couch", [&](std::string_view root, const std::vector<std::string_view> &components) {
        std::call_once(couch_flag, [&]() { hw_reader.sync_load_all(root, components); });
    });

    static UnicodeSvgReader svg_reader;
    static std::once_flag makemeahanzi_flag;
    m.def("global_init_makemeahanzi", [&](std::string_view path) {
        std::call_once(makemeahanzi_flag, [&]() { svg_reader.sync_load_all(path); });
    });

    m.def("generate", [&](const py::args &args, const py::kwargs &kwargs) {
        SourceType source_type = SourceType::HANDWRITING;
        TextType text_type = TextType::LATEX;
        std::string_view text = "error";
        if (kwargs.contains("text")) { text = kwargs["text"].cast<std::string_view>(); }
        if (kwargs.contains("source_type")) { source_type = kwargs["source_type"].cast<SourceType>(); }
        if (kwargs.contains("text_type")) { text_type = kwargs["text_type"].cast<TextType>(); }
#ifdef XGD_DEBUG
        SPDLOG_INFO("text={}", text);
        SPDLOG_INFO("source_type={}", (int32_t) source_type);
        SPDLOG_INFO("text_type={}", (int32_t) text_type);
#endif
        ShapeProvider shape_provider;
        if (SourceType::HANDWRITING == source_type) {
            shape_provider = [&](const UCharType &label) { return hw_reader.select(label); };
        } else if (SourceType::SVG == source_type) {
            shape_provider = [&](const UCharType &label) { return svg_reader.select(label); };
        } else {
            throw std::runtime_error(fmt::format("unknown source_type: {}", (int32_t) source_type));
        }
        bool force_mathjax = SourceType::HANDWRITING != source_type;
        std::shared_ptr<az::pen::PenOp> pen_op;
        if (TextType::SMILES == text_type) {
            StructuralFormulaItem item;
            item.create_from_format(shape_provider, text);
            pen_op = item.data;
        } else if (TextType::LATEX == text_type) {
            auto traverse_order = SynthesisTexGenerator::TraverseOrder::DEFAULT;
            if (kwargs.contains("traverse_order")) {
                traverse_order = kwargs["traverse_order"].cast<SynthesisTexGenerator::TraverseOrder>();
#ifdef XGD_DEBUG
                SPDLOG_INFO("traverse_order{}", (int32_t) traverse_order);
#endif
            }
            pen_op = SynthesisTexGenerator::generate_next(
                    text.data(), shape_provider, [](const UCharType &) {}, force_mathjax, traverse_order);
        } else {
            throw std::runtime_error(fmt::format("unknown text_type: {}", (int32_t) text_type));
        }
        if (auto pen_graph = std::dynamic_pointer_cast<PenGraph>(pen_op)) {
            pen_graph->adjust_size_for_rendering(88, 8192, 1.2);
        }
        if (nullptr == pen_op) {
            throw std::runtime_error("generation failed");
        }
        return pen_op;
    });
    m.attr("__version__") = "dev";
}
