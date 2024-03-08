#include "az/pen/pen_op.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/data/couch_reader.h"
#include "az/pen/pen_graph.h"
#include "az/data/structural_formula_item.h"
#include "az/data/unicode_shape.h"

#include "skia_utils.h"

#include <spdlog/spdlog.h>
#include <gtest/gtest.h>

#include <fstream>

const size_t TOTAL = 60;
const int MAX_WIDTH = 2560;
const int MAX_HEIGHT = 1440;
const int STROKE_WIDTH = 4;
const int ROTATE_DEG = 15;

static void run() {
    using namespace az;
    using namespace az::pen;
    using namespace az::data;
    std::vector<std::string> smi_list = {
            "c1ccccc1",
    };
    if (false) {
//    if (true) {
        std::ifstream in("/home/xgd/source/repos/xgd-project/assets/datasets/drugbank/subset.smi");
        std::string line;
        while (std::getline(in, line)) {
            smi_list.push_back(std::move(line));
        }
    }
    std::sort(smi_list.begin(), smi_list.end(), [](auto &a, auto &b) {
        return a.size() > b.size(); // by length
    });
    CouchDatasetReader reader;
    reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
            "Couch_Digit_195",
//            "Couch_GB1_188",
//            "Couch_GB2_195",
            "Couch_Letter_195",
            "Couch_Symbol_130"
    });
//    SPDLOG_INFO("Ａ={}", az::data::map_unicode_by_label("Ａ").value());
    SynthesisTexGenerator generator;
    std::unordered_set<UCharType> failed_cases;
    const ShapeProvider shape_provider = [&](const UCharType &label) {
        return reader.select(label);
    };
    size_t rendered = 0;
    for (size_t i = 0; i < smi_list.size(); i++) {
        if (++rendered > 20) { break; }
        const auto &smi = smi_list[i];
        SPDLOG_INFO("TEX={}", smi);
        StructuralFormulaItem item;
        item.create_from_format(shape_provider, smi);
        auto &data = item.data;
//        size_t item_count = data->data.size();
//        auto box = data->bbox();
//        const int base_width = std::sqrt(item_count) * 100;
//        const int base_height = base_width * box.sizes().y() / box.sizes().x();
//        const int width = (std::min)(base_width, MAX_WIDTH);
//        const int height = (std::min)(base_height, MAX_HEIGHT);
        const int width = MAX_WIDTH;
        const int height = MAX_HEIGHT;
        data->fit_into_keep_ratio(width - 4 * STROKE_WIDTH, height - 4 * STROKE_WIDTH);
        draw_with_skia(data.get(), "chem.png",//fmt::format("{:06d}.png", i),
                       width, height, STROKE_WIDTH, "");
    }
}

TEST(test_data, tex_mol_generator) {
    run();
}
