#include "az/media/data_output.h"

#include "az/pen/pen_op.h"
#include "az/pen/pen_context.h"
#include "az/data/structural_formula_item.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/data/couch_reader.h"
#include "az/data/makemeahanzi_reader.h"
#include "az/math/multiply_draft.h"


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <taskflow/taskflow.hpp>

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

    const auto generate_single = [&](
            std::string_view text,
            SourceType source_type,
            TextType text_type,
            SynthesisTexGenerator::TraverseOrder traverse_order
    ) {
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
    };
    m.def("generate", [&](const py::args &args, const py::kwargs &kwargs) {
        std::string_view text = "error";
        SourceType source_type = SourceType::HANDWRITING;
        TextType text_type = TextType::LATEX;
        auto traverse_order = SynthesisTexGenerator::TraverseOrder::DEFAULT;

        if (kwargs.contains("text")) { text = kwargs["text"].cast<std::string_view>(); }
        if (kwargs.contains("source_type")) { source_type = kwargs["source_type"].cast<SourceType>(); }
        if (kwargs.contains("text_type")) { text_type = kwargs["text_type"].cast<TextType>(); }
        if (kwargs.contains("traverse_order")) {
            traverse_order = kwargs["traverse_order"].cast<SynthesisTexGenerator::TraverseOrder>();
        }

        return generate_single(text, source_type, text_type, traverse_order);
    });
    // trick: do not pass py::xxx to another thread
    m.def("generate_batch", [&](const py::args &args, const py::kwargs &kwargs) {
        SourceType source_type = SourceType::HANDWRITING;
        TextType text_type = TextType::LATEX;
        auto traverse_order = SynthesisTexGenerator::TraverseOrder::DEFAULT;
        auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 192u);
        if (kwargs.contains("source_type")) { source_type = kwargs["source_type"].cast<SourceType>(); }
        if (kwargs.contains("text_type")) { text_type = kwargs["text_type"].cast<TextType>(); }
        if (kwargs.contains("traverse_order")) {
            traverse_order = kwargs["traverse_order"].cast<SynthesisTexGenerator::TraverseOrder>();
        }
        if (kwargs.contains("parallel_num")) {
            int tmp = kwargs["parallel_num"].cast<int>();
            if (tmp > 0) {
                thread_num = tmp;
            }
        }
        auto texts = kwargs["texts"].cast<std::vector<std::string_view>>();

        PointerVec<PenOp> outputs;
        outputs.reserve(texts.size());
        outputs.resize(texts.size());
        tf::Taskflow taskflow;
        taskflow.for_each_index(size_t{0}, texts.size(), size_t{1}, [&](size_t i) {
            outputs[i] = generate_single(texts[i], source_type, text_type, traverse_order);
        });
        tf::Executor(thread_num).run(taskflow).get();
        return outputs;
    });

    using fn_media_callback = std::function<
            std::vector<uint8_t>(
                    az::pen::PenOp *data,
                    const az::media::PenOpPaintParam &param
            )
    >;
    const auto generate_batch_media = [&](const py::args &args, const py::kwargs &kwargs,
                                          const fn_media_callback &on_media) {
        SourceType source_type = SourceType::HANDWRITING;
        TextType text_type = TextType::LATEX;
        auto traverse_order = SynthesisTexGenerator::TraverseOrder::DEFAULT;
        int frame_width = -1;
        int frame_height = -1;
        auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 192u);
        if (kwargs.contains("source_type")) { source_type = kwargs["source_type"].cast<SourceType>(); }
        if (kwargs.contains("text_type")) { text_type = kwargs["text_type"].cast<TextType>(); }
        if (kwargs.contains("traverse_order")) {
            traverse_order = kwargs["traverse_order"].cast<SynthesisTexGenerator::TraverseOrder>();
        }
        if (kwargs.contains("frame_width")) {
            int tmp = kwargs["frame_width"].cast<int>();
            if (tmp > 0) {
                frame_width = tmp;
            }
        }
        if (kwargs.contains("frame_height")) {
            int tmp = kwargs["frame_height"].cast<int>();
            if (tmp > 0) {
                frame_height = tmp;
            }
        }
        if (kwargs.contains("parallel_num")) {
            int tmp = kwargs["parallel_num"].cast<int>();
            if (tmp > 0) {
                thread_num = tmp;
            }
        }
        auto texts = kwargs["texts"].cast<std::vector<std::string_view>>();

        std::vector<std::vector<uint8_t>> outputs;
        outputs.reserve(texts.size());
        outputs.resize(texts.size());
        tf::Taskflow taskflow;
        static const size_t MAX_RENDER_SIZE = 8192;
        static const az::ScalarType IDEAL_ITEM_SIZE = 88;
        static const az::ScalarType STROKE_WIDTH = 3.;
        static const az::ScalarType BLANK_PADDING_RATIO = 1.2;
        taskflow.for_each_index(size_t{0}, texts.size(), size_t{1}, [&](size_t i) {
            auto pen_op = generate_single(texts[i], source_type, text_type, traverse_order);
            ScalarType width = 64, height = 64;
            if (auto pen_graph = std::dynamic_pointer_cast<PenGraph>(pen_op)) {
                auto size = pen_graph->adjust_size_for_rendering(
                        IDEAL_ITEM_SIZE, MAX_RENDER_SIZE / BLANK_PADDING_RATIO, BLANK_PADDING_RATIO);
                width = size.x();
                height = size.y();
            }
            if (!((0 < width && width < MAX_RENDER_SIZE + 1) && (0 < height && height < MAX_RENDER_SIZE + 1))) {
                SPDLOG_ERROR("skip invalid render size={}x{}", width, height);
                width = 64;
                height = 64;
            }
            int render_width = std::ceil(width);
            int render_height = std::ceil(height);
            if (render_width < frame_width && render_height < frame_height) {
                render_width = frame_width;
                render_height = frame_height;
                pen_op->fit_into_keep_ratio(render_width / BLANK_PADDING_RATIO, render_height / BLANK_PADDING_RATIO);
                pen_op->move_center_to(Vec2{render_width, render_height} / 2);
            }
            outputs[i] = on_media(pen_op.get(), az::media::PenOpPaintParam{
                    .render_width=render_width,
                    .render_height=render_height,
                    .stroke_width=STROKE_WIDTH,
                    .frame_width=frame_width < 0 ? render_width : frame_width,
                    .frame_height=frame_height < 0 ? render_height : frame_height,
                    .stroke_color=0xFFFFFFFF,
                    .bg_color=0xFF000000,
            });
        });
        tf::Executor(thread_num).run(taskflow).get();
        std::vector<py::bytes> py_outputs;
        py_outputs.reserve(outputs.size());
        for (auto &output: outputs) {
            py_outputs.emplace_back((char *) output.data(), output.size());
        }
        return py_outputs;
    };
    m.def("generate_batch_image", [&](const py::args &args, const py::kwargs &kwargs) {
        return generate_batch_media(args, kwargs, az::media::pen_op_to_image);
    });

    m.def("generate_batch_video", [&](const py::args &args, const py::kwargs &kwargs) {
        return generate_batch_media(args, kwargs, az::media::pen_op_to_video);
    });

    m.def("generate_multiply_draft_latex", [](std::string_view a, std::string_view b) {
        return az::math::MultiplyDraft::generate(a, b);
    });

    m.attr("__version__") = "dev";
}
