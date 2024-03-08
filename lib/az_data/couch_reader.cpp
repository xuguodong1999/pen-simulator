#include "az/data/couch_reader.h"
#include "az/data/unicode_shape.h"
#include "az/core/utils.h"

#include "couch_raw_item.h"

#include <taskflow/taskflow.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <filesystem>
#include <algorithm>

using namespace az;
using namespace az::data;
using az::pen::PenChar;

namespace fs = std::filesystem;

std::vector<std::string_view> CouchDatasetReader::COUCH_SUB_DIRS = {
        "Couch_Digit_195",
        "Couch_GB1_188",
        "Couch_GB2_195",
        "Couch_Letter_195",
        "Couch_Symbol_130"
};

static void read_idx_batched(
        const std::vector<fs::path> &path_list,
        CouchDatasetReader::DataSourceType &data_source
) {
    tf::Taskflow taskflow;
    const auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 16u);
    std::mutex mutex;
    taskflow.for_each_index(size_t{0}, path_list.size(), size_t{1}, [&](size_t i) {
        const fs::path &file_info = path_list[i];

        std::string path_no_suffix = file_info.parent_path().string();
        path_no_suffix += fs::path::preferred_separator;
        path_no_suffix += file_info.stem().string();

        std::ifstream idx_in{path_no_suffix + ".idx", std::ios::in | std::ios::binary};
        std::ifstream dat_in{path_no_suffix + ".dat", std::ios::in | std::ios::binary};
        if (!dat_in.is_open()) { // there is an exception at Couch_GB1_188/GB1_049.DAT
            dat_in.open(path_no_suffix + ".DAT", std::ios::in | std::ios::binary);
        }
        if (!(idx_in.is_open() && dat_in.is_open())) {
            throw std::runtime_error(fmt::format("fail to open {}", path_no_suffix));
        }
        impl::CouchRawItem raw_item;
        PointerVec<CouchItem> samples = raw_item.read(idx_in, dat_in);
        idx_in.close();
        dat_in.close();
        {
            std::unique_lock lk(mutex);
            for (auto &sample: samples) {
                const auto maybe_label = az::data::map_unicode_by_label(sample->label);
//                const auto maybe_label = az::data::map_unicode_by_shape(sample->label);
                const auto &label = maybe_label ? maybe_label.value() : sample->label;
                insert_unordered_map_vector(data_source, label, sample);
            }
        }
    });
    const auto beg = std::chrono::high_resolution_clock::now();
    tf::Executor(thread_num).run(taskflow).get();
    const auto end = std::chrono::high_resolution_clock::now();
    SPDLOG_INFO("use {} threads cost {:.3f} ms", thread_num,
                std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count() / 1000.f);
}


void CouchDatasetReader::sync_load_all(
        std::string_view couch_dir,
        const std::vector<std::string_view> &sub_dirs
) {
    fs::path root = couch_dir;
    root += fs::path::preferred_separator;
    std::vector<fs::path> files;
    for (const auto &sub_dir: sub_dirs) {
        fs::path dir = root;
        dir += sub_dir;
        std::vector<fs::path> sub_dir_files = scan_files_by_ext(dir, ".idx");
        SPDLOG_INFO("{} contain {} idx files", dir.string(), sub_dir_files.size());
        move_append(files, sub_dir_files);
    }
    read_idx_batched(files, data_source);
    keys.reserve(data_source.size());
    for (auto &[label, bucket]: data_source) {
        keys.push_back(label);
    }
    SPDLOG_INFO("load {} kind of chars", keys.size());
}

PenChar *CouchDatasetReader::select(const UCharType &label) {
    PointerVec<CouchItem> *bucket = nullptr;
    auto it = data_source.find(label);
    if (it != data_source.end()) {
        bucket = &(it->second);
    }
    if (!bucket) { return nullptr; }
    if (bucket->empty()) {
        throw std::runtime_error(fmt::format("select: empty bucket for label \"{}\"", label));
    }
    return &(random_select(*bucket)->data);
}

std::pair<PenChar *, UCharType *> CouchDatasetReader::next() {
    if (keys.empty()) {
        throw std::runtime_error("next: empty keys");
    }
    auto &random_key = random_select(keys);
    PointerVec<CouchItem> &bucket = data_source[random_key];
    if (bucket.empty()) {
        throw std::runtime_error("next: empty bucket");
    }
    auto &item = random_select(bucket);
    return {&item->data, &item->label};
}

SizeType CouchDatasetReader::size() const {
    SizeType size = 0;
    for (auto &[label, bucket]: data_source) {
        size += bucket.size();
    }
    return size;
}

const std::vector<UCharType> &CouchDatasetReader::get_keys() const {
    return keys;
}

void CouchDatasetReader::binary_serialize(std::ostream &out) {

}

void CouchDatasetReader::binary_deserialize(std::istream &in) {

}
