#include "az/data/inkml_reader.h"
#include "az/data/unicode_shape.h"
#include "az/core/utils.h"

#include "inkml_raw_item.h"

#include <taskflow/taskflow.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <random>
#include <filesystem>
#include <algorithm>

using namespace az;
using namespace az::data;
using namespace az::pen;

namespace fs = std::filesystem;

std::vector<std::string_view> InkmlDatasetReader::HME23_SUB_DIRS = {
        "WebData_CROHME23_new_v2.3/new_train",
        "WebData_CROHME23_new_v2.3/new_val",
        "WebData_CROHME23_new_v2.3/Syntactic_data/INKML/gen_LaTeX_data_CROHME_2019",
        "WebData_CROHME23_new_v2.3/Syntactic_data/INKML/gen_LaTeX_data_CROHME_2023_corpus",
        "WebData_CROHME23_new_v2.3/Syntactic_data/INKML/gen_syntactic_data",
};


void InkmlDatasetReader::sync_load_all(
        std::string_view root_dir,
        const std::vector<std::string_view> &sub_dirs
) {
    fs::path root = root_dir;
    root += fs::path::preferred_separator;
    std::vector<fs::path> path_list;
    for (const auto &sub_dir: sub_dirs) {
        fs::path dir = root;
        dir += sub_dir;
        std::vector<fs::path> sub_dir_files = scan_files_by_ext(dir.c_str(), ".inkml");
        SPDLOG_INFO("{} contain {} inkml files", dir.string(), sub_dir_files.size());
        move_append(path_list, sub_dir_files);
    }
    tf::Taskflow taskflow;
    const auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 16u);
    std::mutex mutex_sis;
    std::mutex mutex_ds;
    taskflow.for_each_index(size_t{0}, path_list.size(), size_t{1}, [&](size_t i) {
        const fs::path &file_info = path_list[i];
        impl::InkmlRawItem raw_item;
        std::ifstream in(file_info);
        InkmlItem item = raw_item.read(in, [&](const PenChar &c, const LabelType &label) {
            PenChar copied_char = c;
            {
                std::unique_lock lk(mutex_sis);
                insert_unordered_map_vector(single_item_source, label, std::move(copied_char));
            }
        });
        {
            std::unique_lock lk(mutex_ds);
            data_source.push_back(std::move(item));
        }
    });
    const auto beg = std::chrono::high_resolution_clock::now();
    tf::Executor(thread_num).run(taskflow).get();
    const auto end = std::chrono::high_resolution_clock::now();
    SPDLOG_INFO("use {} threads cost {:.3f} ms", thread_num,
                std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count() / 1000.f);

    SPDLOG_INFO("load {} kinds of chars", single_item_source.size());
    size_t total = 0;
    for (auto &[label, bucket]: single_item_source) {
        total += bucket.size();
    }
    SPDLOG_INFO("load {} samples", total);
}

PenChar *InkmlDatasetReader::select(const LabelType &label) {
    std::vector<PenChar> *bucket = nullptr;
    auto it = single_item_source.find(label);
    if (it != single_item_source.end()) {
        bucket = &(it->second);
    }
    if (!bucket) { return nullptr; }
    if (bucket->empty()) {
        throw std::runtime_error(fmt::format("select: empty bucket on \"{}\"", label));
    }
    return &random_select(*bucket);
}

#define  SEQ_NEXT

std::pair<PenString *, LabelType *> InkmlDatasetReader::next() {
#ifdef SEQ_NEXT
    static size_t i = 0;
    if (i == data_source.size()) { i = 0; }
    auto &item = data_source[i++];
#else
    auto &bucket = data_source;
    if (bucket.empty()) {
        throw std::runtime_error("next: empty bucket");
    }
    auto &item = random_select(bucket);
#endif
    return {&item.data, &item.latex};
}

SizeType InkmlDatasetReader::size() const {
    return data_source.size();
}

void InkmlDatasetReader::binary_serialize(std::ostream &out) {

}

void InkmlDatasetReader::binary_deserialize(std::istream &in) {

}

void InkmlDatasetReader::get_keys(
        const std::function<void(const LabelType &)> &on_key) const {
    for (auto &[label, _]: single_item_source) {
        on_key(label);
    }
}
