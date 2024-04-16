#include "utils.h"

#include "az/core/init_spdlog.h"

#include "az/data/couch_reader.h"
#include "az/data/makemeahanzi_reader.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/data/structural_formula_item.h"

#include "az/math/multiply_draft.h"
#include "az/math/alkane_isomer.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <taskflow/taskflow.hpp>

#include <string>
#include <vector>
#include <limits>
#include <unordered_set>

namespace po = boost::program_options;

using namespace az;
using namespace az::pen;
using namespace az::data;

using SourceType = std::string;
using TextType = std::string;

using InputItemType = std::tuple<LabelType, SourceType, TextType>;
using InputListType = std::vector<InputItemType>;

static void add_help_info(po::options_description &desc);

static InputListType parse_input_json(boost::property_tree::ptree &pt);

static InputListType generate_random_multiply_samples(const int &count);

static InputListType generate_full_alkane_isomer_samples(const int8_t &carbon_num);

static const std::unordered_set<SourceType> supported_source_type = {"handwriting", "svg"};
static const std::unordered_set<TextType> supported_text_type = {"smi", "latex"};

static const size_t MAX_RENDER_SIZE = 8192;
static const az::ScalarType IDEAL_ITEM_SIZE = 88;
static const az::ScalarType STROKE_WIDTH = 8.;
static const az::ScalarType BLANK_PADDING_RATIO = 1.2;

int main(int argc, char **argv) {
    std::srand(0022); // NOLINT
    /// 1. parse arguments
    az::core::setup_spdlog();
    po::options_description desc(R"(
---------------------------------------------------------------------------
az-data-cli is a visualization tool for svg and handwriting data synthesis.
az-data-cli supports trace of svg paths and handwriting points.
---------------------------------------------------------------------------
Usage)", 120);
    add_help_info(desc);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        SPDLOG_INFO(desc);
        return EXIT_SUCCESS;
    }

    /// 2. parse input
    InputListType input_list;
    auto multiply_count = vm.at("multiply").as<int>();
    auto carbon_num = vm.at("isomer").as<int>();
    if (multiply_count > 0) {
        input_list = generate_random_multiply_samples(multiply_count);
    } else if (carbon_num > 0) {
        if (carbon_num > 32) {
            SPDLOG_ERROR("carbon num {} > 32 is not supported under uint64_t", carbon_num);
            return EXIT_SUCCESS;
        } else if (carbon_num > 23) {
            std::string yes;
            SPDLOG_WARN("C {} is too large, generation is slow, OOM may happen, are you sure to continue? (yes/no)");
            std::getline(std::cin, yes);
            if (yes != "yes") {
                SPDLOG_WARN("user denied with '{}' != 'yes'", yes);
                return EXIT_SUCCESS;
            }
        }
        input_list = generate_full_alkane_isomer_samples((int8_t) carbon_num);
    } else {
        const auto input = vm.at("input").as<std::string>();
        SPDLOG_INFO("input={}", input);
        std::ifstream in(input);
        if (!in.is_open()) {
            SPDLOG_ERROR("failed to open {}", input, input_list.size());
            SPDLOG_INFO(desc);
            return EXIT_FAILURE;
        }
        /// 3. parse json
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(in, pt);
        in.close();
        input_list = parse_input_json(pt);
    }

    /// 4. create svg data source
    UnicodeSvgReader svg_reader;
    const auto makemeahanzi_path = vm.at("makemeahanzi").as<std::string>();
    SPDLOG_INFO("makemeahanzi path={}", makemeahanzi_path);
    try {
        svg_reader.sync_load_all(makemeahanzi_path);
    } catch (std::exception &e) {
        SPDLOG_ERROR(e.what());
        SPDLOG_INFO(desc);
        return EXIT_FAILURE;
    }

    /// 5. create handwriting data source
    CouchDatasetReader hw_reader;
    const auto couch_path = vm.at("couch").as<std::string>();
    SPDLOG_INFO("couch path={}", couch_path);
    try {
        hw_reader.sync_load_all(couch_path, {
                "Couch_Digit_195",
//                "Couch_GB1_188",
//                "Couch_GB2_195",
                "Couch_Letter_195",
                "Couch_Symbol_130"
        });
    } catch (std::exception &e) {
        SPDLOG_ERROR(e.what());
        SPDLOG_INFO(desc);
        return EXIT_FAILURE;
    }

    /// 6. create scheduler of shape provider
    const auto scheduler = [&](
            const LabelType &content, const SourceType &source_type, const TextType &text_type) {
        ShapeProvider shape_provider;
        if ("handwriting" == source_type) {
            shape_provider = [&](const UCharType &label) { return hw_reader.select(label); };
        } else if ("svg" == source_type) {
            shape_provider = [&](const UCharType &label) { return svg_reader.select(label); };
        }
        bool force_mathjax = "handwriting" != source_type;
        std::shared_ptr<az::pen::PenOp> pen_op;
        if ("smi" == text_type) {
            StructuralFormulaItem item;
            item.create_from_format(shape_provider, content, "smi", carbon_num > 0);
            pen_op = item.data;
        } else if ("latex" == text_type) {
            pen_op = SynthesisTexGenerator::generate_next(
                    content, shape_provider, [](const UCharType &) {}, force_mathjax,
                    multiply_count ? SynthesisTexGenerator::TraverseOrder::SORT_BY_MULTIPLY_DEMONSTRATION
                                   : SynthesisTexGenerator::TraverseOrder::DEFAULT);
        }
        return pen_op;
    };

    /// 7. generate samples
    const auto output_directory = vm.at("output").as<std::string>();
    bool show_gui = output_directory.empty();

    tf::Taskflow taskflow;
    size_t limit = (std::min)(input_list.size(), vm.at("limit").as<size_t>());
    taskflow.for_each_index(size_t{0}, limit, size_t{1}, [&](size_t i) {
        const auto &[content, source_type, text_type] = input_list[i];
        SPDLOG_INFO("handle [{}][{}]: \n******\n{}\n******", source_type, text_type, content);
        auto pen_op = scheduler(content, source_type, text_type);
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
        } else {
            SPDLOG_INFO("render size={}x{}", width, height);
        }
        if (show_gui) {
            display_in_opencv_highgui(
                    pen_op.get(),
                    std::ceil(width), std::ceil(height),
                    STROKE_WIDTH,
                    content,
                    720, 480
//                    1720, 720
            );
        } else {
            save_frames(
                    pen_op.get(),
                    std::ceil(width), std::ceil(height),
                    STROKE_WIDTH,
                    output_directory,
                    fmt::format("{}", i),
//                    2560, 1440
                    1280, 1280
            );
        }
    });
    tf::Executor(show_gui ? 1 : vm.at("threads").as<size_t>()).run(taskflow).get();
    return EXIT_SUCCESS;
}

void add_help_info(po::options_description &desc) {
    desc.add_options()
            ("help,h", "print usage of az-data-cli")
            ("couch", po::value<std::string>()->default_value("Couch"),
             R"(path to SCUT_IRAC/Couch dataset.
see www.hcii-lab.net/data/scutcouch/cn/download.html
----------------------------------------------------
SCUT_IRAC
└── Couch (point to this directory)
    ├── Couch_Digit_195
    ├── Couch_GB1_188
    ├── Couch_GB2_195
    ├── Couch_Letter_195
    └── Couch_Symbol_130
)")
            ("makemeahanzi", po::value<std::string>()->default_value("graphics.txt"),
             R"(path to makemeahanzi graphics.txt.
see https://github.com/skishore/makemeahanzi
--------------------------------------------
makemeahanzi
└── graphics.txt (point to this file)
)")
            ("input", po::value<std::string>()->default_value("example.json"),
             "path to a json file containing inputs,  \nsee apps/az_data_cli/example.json for usage")
            ("output", po::value<std::string>()->default_value(""),
             "path to output frames directory, \ndisplay with opencv highgui if left empty")
            ("threads", po::value<size_t>()->default_value(8),
             "when output is specified, run in parallel")
            ("limit", po::value<size_t>()->default_value(std::numeric_limits<size_t>::max()),
             "if samples in json exceed limit, break generation")
            ("multiply", po::value<int>()->default_value(0),
             "if > 0, ignore input and display number multiply examples")
            ("isomer", po::value<int>()->default_value(0),
             "ignore input and display alkane isomer examples");
}

InputListType parse_input_json(boost::property_tree::ptree &pt) {
    // [{content, source_type, type}, ...]
    InputListType input_list;
    for (auto &[k, v]: pt) {
        if (v.get_child_optional("description")) {
            continue;
        }
        TextType text_type;
        LabelType content;
        SourceType source_type;
        try {
            text_type = v.get_child("text_type").data();
            source_type = v.get_child("source_type").data();
            content = v.get_child("content").data();
        } catch (std::exception &e) {
            SPDLOG_ERROR("unexpected json: {}", e.what());
            continue;
        }
        if (!supported_source_type.contains(source_type)) {
            SPDLOG_ERROR("unexpected source_type: {}", source_type);
            continue;
        } else if (!supported_text_type.contains(text_type)) {
            SPDLOG_ERROR("unexpected text_type: {}", text_type);
            continue;
        }
        input_list.emplace_back(content, source_type, text_type);
    }
    return input_list;
}

InputListType generate_full_alkane_isomer_samples(const int8_t &carbon_num) {
    InputListType input_list;
    auto smiles = az::math::AlkaneIsomerUtil::get_isomers_sync(carbon_num);
    input_list.reserve(2 * smiles.size());
    for (auto &smi_hash: smiles) {
        std::string smi = az::math::AlkaneIsomerUtil::hash_to_smi(smi_hash);
        input_list.emplace_back(smi, "handwriting", "smi");
        input_list.emplace_back(smi, "svg", "smi");
    }
    return input_list;
}

InputListType generate_random_multiply_samples(const int &count) {
    // [{content, source_type, type}, ...]
    InputListType input_list;
    const auto number_generator = []() {
        // width: 2~4, dot anywhere
        auto length = random_int(2, 4);
        auto buffer = std::to_string(random_int(1, int(std::pow(10, length) - 1)));
        auto pos = random_int(0, int(buffer.length()));
        buffer.insert(pos, ".");
        return buffer;
    };
    input_list.reserve(2 * count);
    for (int i = 0; i < count; i++) {
        auto a = number_generator();
        auto b = number_generator();
        auto latex = az::math::MultiplyDraft::generate(a, b);
        input_list.emplace_back(latex, "handwriting", "latex");
        input_list.emplace_back(latex, "svg", "latex");
    }
    return input_list;
}
