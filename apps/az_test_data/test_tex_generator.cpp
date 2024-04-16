#include "az/pen/pen_op.h"
#include "az/data/synthesis_tex_generator.h"
#include "az/data/couch_reader.h"
#include "az/data/makemeahanzi_reader.h"
#include "az/pen/pen_graph.h"
#include "az/data/inkml_reader.h"
#include "az/data/im2latex_reader.h"
#include "az/data/unicode_shape.h"

#include "utils.h"
#include <spdlog/spdlog.h>
#include <gtest/gtest.h>

const size_t TOTAL = 60;
const int MAX_WIDTH = 2560;
const int MAX_HEIGHT = 1440;
const int STROKE_WIDTH = 4;
const int ROTATE_DEG = 15;

static void run(
        const std::vector<std::string> &tex_list,
        const std::string_view &name = "",
        bool display_mode = true
) {
    using namespace az;
    using namespace az::pen;
    using namespace az::data;
    UnicodeSvgReader printed_reader;
    printed_reader.sync_load_all("/home/xgd/git/_/web/makemeahanzi/graphics.txt");
//    for (auto &k: printed_reader.get_keys()) {
//        SPDLOG_INFO("key={}", k);
//    }
//    return;
    const ShapeProvider printed_shape_provider = [&](const UCharType &label) {
        return printed_reader.select(label);
    };

    CouchDatasetReader hw_reader;
    hw_reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
            "Couch_Digit_195",
            "Couch_GB1_188",
            "Couch_GB2_195",
            "Couch_Letter_195",
            "Couch_Symbol_130"
    });
    const ShapeProvider hw_shape_provider = [&](const UCharType &label) {
        return hw_reader.select(label);
    };
//    SPDLOG_INFO("Ａ={}", az::data::map_unicode_by_label("Ａ").value());
    std::unordered_set<UCharType> failed_cases;
    for (size_t i = 0; i < tex_list.size(); i++) {
        const auto &tex = tex_list[i];
        for (int j = 0; j < 2; j++) {
            SPDLOG_INFO("TEX={}", tex);
            auto data = SynthesisTexGenerator::generate_next(
                    tex,
//                    j == 0 ? hw_shape_provider : printed_shape_provider,
//                    hw_shape_provider,
                    printed_shape_provider,
                    [&](const UCharType &label) {
                        failed_cases.insert(label);
                    });
//            size_t item_count = data->data.size();
//            const int base_width = item_count * 40;
//            const int base_height = base_width * MAX_HEIGHT / MAX_WIDTH;
//            const int width = (std::min)(base_width, MAX_WIDTH);
//            const int height = (std::min)(base_height, MAX_HEIGHT);
            const int width = MAX_WIDTH;
            const int height = MAX_HEIGHT;
            data->fit_into_keep_ratio(width - 4 * STROKE_WIDTH, height - 4 * STROKE_WIDTH);
            std::string task_name;
            if (name.empty()) {
                task_name = fmt::format("{:06d}.png", i);
            } else {
                task_name = name;
            }
            draw_with_skia(data.get(), task_name, width, height, STROKE_WIDTH, tex);
        }
    }
    {
        std::ostringstream out;
        for (auto &failed_case: failed_cases) {
            out << failed_case << ",";
        }
        SPDLOG_INFO("failed: {}", out.str());
    }
}

static void run_crohme_2023(bool display_mode) {
    using namespace az;
    using namespace az::data;
    std::vector<std::string> tex_list;
    InkmlDatasetReader reader;
    reader.sync_load_all(get_dataset_root("WebData_CROHME23"), {
            "WebData_CROHME23_new_v2.3/new_train",
            "WebData_CROHME23_new_v2.3/new_val",
    });
    std::ostringstream out;
    size_t total = reader.size();
    tex_list.reserve(total);
    for (size_t i = 0; i < total; i++) {
        auto [raw_data, label] = reader.next();
        tex_list.push_back(*label);
    }
    std::sort(tex_list.begin(), tex_list.end(), [](auto &a, auto &b) {
        return a.size() > b.size(); // by length
    });
    run(tex_list, "", display_mode);
}

static void run_latex_1(bool display_mode) {
    std::vector<std::string> tex_list = {R"(
\begin{array} {rl}{\delta \over \delta S^{\mu \nu}(z(\tau))} &        E_{\rm conf}(\bar z - \bar z^\prime,         \dot{\bar z}, \dot{\bar z} ^\prime) = \nonumber \\= & \sigma \delta (\tau - \bar \tau)       \delta(z_0 - \bar z_0^\prime)       \epsilon(\dot z_0) \dot{\bar z} _0^\prime       {(\bar z^\mu - \bar z^{\prime \mu}) \dot{\bar z} ^\nu -      (\bar z ^\nu - \bar z^{\prime \nu})\dot {\bar z} ^\mu       \over | \bar {\vec z} - \bar{\vec z}^\prime|       \sqrt {\dot {\bar z_0}^2 -       \dot {\bar {\vec z}}_{\rm T}^2}} - \nonumber \\ & \qquad \qquad - \sigma \delta (\tau - \bar \tau ^\prime)        \delta(\bar z_0 -        \bar z_0^\prime) \epsilon(\dot z_0^\prime) \dot{\bar z} _0       {(\bar z^\mu - \bar z^{\prime \mu}) \dot{\bar z} ^{\prime \nu} -      (\bar z ^\nu - \bar z^{\prime \nu})\dot {\bar z} ^{\prime \mu}       \over | \bar {\vec z} - \bar{\vec z}^\prime|       \sqrt {\dot {\bar z_0}^{\prime 2} -       \dot {\bar {\vec z}}_{\rm T}^{\prime 2}}} \end{array}
)"};
    run(tex_list, "", display_mode);
}

static void run_latex_2(bool display_mode) {
    std::vector<std::string> tex_list = {R"(
\displaylines{
[😅]+[😅][😅]=[😅][😅][😅] \\
[😅]\times[😅][😅]=[😅][😅]
}
)", R"(
\displaylines{
1+2=3 \\
4+5=9
}
)", R"(
\begin{pmatrix}
 你好 & c_1 & c_1^2 & \cdots & c_1^n \\
 😅 & c_2 & c_2^2 & \cdots & c_2^n \\
 \vdots  & \vdots& \vdots & \ddots & \vdots \\
 1 & c_m & c_m^2 & \cdots & c_m^n \\
\end{pmatrix}
)", R"(
\begin{pmatrix}
 你好 & c_1 & c_1^2 & \cdots & c_1^n \\
 😅 & c_2 & c_2^2 & \cdots & c_2^n \\
 \vdots  & \vdots& \vdots & \ddots & \vdots \\
 1 & c_m & c_m^2 & \cdots & c_m^n \\
 \sum_{n=1}^{\infty} \frac{1}{3^n \cdot n} & \sum_{n=1}^{\infty} \frac{1}{4^n \cdot n} & 😅 & 😅 & 😅 \\
 \frac{2}{5} \sum_{k=0}^{\infty} \frac{1}{25^k(2 k+1)} & \frac{2}{7} \sum_{k=0}^{\infty} \frac{1}{49^k(2 k+1)} & 😅 & 😅 & 😅 \\
 7 \sum_{n=1}^{\infty} \frac{1}{16^n \cdot n} & 5 \sum_{n=1}^{\infty} \frac{1}{25^n \cdot n} &  \sum_{n=1}^{\infty} \frac{1}{81^n \cdot n} & 😅 & 😅
\end{pmatrix}
)",
    };
    run(tex_list, "", display_mode);
}

static void run_im2latex(bool display_mode) {
    using namespace az;
    using namespace az::data;
    std::vector<std::string> im2latex_list;
    std::vector<std::string> tex_list;
    Im2LatexReader reader;
    reader.sync_load_all(get_dataset_root("im2latex/raw/im2latex_formulas.lst.json"));
    size_t total = reader.size();
    im2latex_list.reserve(total);
    while (total-- != 0) {
        auto [latex, isvalid] = reader.next();
        if (!(*isvalid)) {
            break;
        }
        im2latex_list.push_back(*latex);
    }
    std::sort(im2latex_list.begin(), im2latex_list.end(), [](auto &a, auto &b) {
        return a.size() > b.size(); // by length
    });
    std::unordered_set<size_t> valid_idx_set = {6, 8, 15, 26};
    tex_list.reserve(valid_idx_set.size());
    for (auto i: valid_idx_set) {
        tex_list.push_back(im2latex_list[i]);
    }
    run(tex_list, "", display_mode);
}

static void run_qa(bool display_mode) {
    run({
                R"(
\displaylines{
问\ \ 正余正加正？\\
答\ \ \sin(\alpha)\cos(\beta)=\frac{\sin(\alpha+\beta)+\sin(\alpha-\beta)}{2}
}
)",
                R"(
\displaylines{
问\ \ 余正正减正？\\
答\ \ \cos(\alpha)\sin(\beta)=\frac{\sin(\alpha+\beta)-\sin(\alpha-\beta)}{2}
}
)",
                R"(
\displaylines{
问\ \ 余余余加余？\\
答\ \ \cos(\alpha)\cos(\beta)=\frac{\cos(\alpha+\beta)+\cos(\alpha-\beta)}{2}
}
)",
                R"(
\displaylines{
问\ \ 正正负的余减余？\\
答\ \ \sin(\alpha)\sin(\beta)=-\frac{\cos(\alpha+\beta)-\cos(\alpha-\beta)}{2}
}
)",

        }, "qa.png", display_mode);
}

static void run_chinese(bool display_mode) {
    run({
                R"(
\displaylines{
美丽的梦和美丽的诗一样\ \ 都是可遇而不可求的 \\
常常在最没能料到的时刻里出现
}
)",
                R"(
\displaylines{
我喜欢那样的梦 \\
在梦里\ \ 一切都可以重新开始\ \ 一切都可以慢慢解释 \\
心里甚至还能感觉到\ \ 所有被浪费的时光 \\
竟然都能重回时的\ \ 狂喜和感激 \\
胸怀中满溢着幸福 \\
只因为你就在我眼前\ \ 对我微笑\  \ 一如当年
}
)",
                R"(
\displaylines{
我真喜欢那样的梦 \\
明明知道你已为我跋涉千里 \\
却又觉得芳草鲜美\ \ 落英缤纷 \\
好像你我才初相遇
}
)",
        }, "chinese.png", display_mode);
}

TEST(test_data, tex_generator_display) {
//    run_chinese(true);
    run_qa(true);
//    run_im2latex(true);
}

TEST(test_data, tex_generator) {
    run_chinese(false);
}
