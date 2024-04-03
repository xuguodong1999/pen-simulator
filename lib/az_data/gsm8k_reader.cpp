#include "az/data/gsm8k_reader.h"

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/exception.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <cassert>
#include <span>

using namespace az;
using namespace az::data;

std::vector<std::string_view> GSM8kReader::GSM8K_FILES = {
        "main/train-00000-of-00001.parquet",
        "main/test-00000-of-00001.parquet",
        "socratic/train-00000-of-00001.parquet",
        "socratic/test-00000-of-00001.parquet",
};

static void convert_chain_thought_to_qa_bean(GSM8kItem &dst, const std::span<std::string_view> &steps) {
    dst.chain_thought_steps.reserve(steps.size());
    for (auto &step: steps) {
        std::vector<std::string_view> sections;
        boost::algorithm::split_regex(sections, step, boost::regex(R"(\*\*)"));
        for (auto &section: sections) {
            section = boost::algorithm::trim_copy_if(section, [](auto &&x) {
                return std::isspace(x) || x == ',';
            });
        }
        std::string_view q, a;
        if (2 == sections.size()) {
            q = sections[0];
            a = sections[1];
        } else if (1 == sections.size()) {
            a = sections[0];
        } else {
            throw std::runtime_error(fmt::format("unhandled data format: {}", step));
        }
        dst.chain_thought_steps.emplace_back(q, a);
    }
}

static void convert_table_to_qa_bean(GSM8kItem &dst, const arrow::Table &row) {
    auto q_ptr = std::dynamic_pointer_cast<arrow::StringArray>(row.GetColumnByName("question")->chunk(0));
    assert(q_ptr);
    auto a_ptr = std::dynamic_pointer_cast<arrow::StringArray>(row.GetColumnByName("answer")->chunk(0));
    assert(a_ptr);
    dst.question = (*q_ptr)[0].value();
    std::vector<std::string_view> sections;
    boost::algorithm::split_regex(sections, (*a_ptr)[0].value(), boost::regex(R"(\n|####)"));
    sections.erase(std::remove_if(sections.begin(), sections.end(), [](auto &&section) {
        return section.empty();
    }), sections.end());
    assert(!sections.empty());
    for (auto &section: sections) {
        section = boost::algorithm::trim_copy(section);
    }
    dst.answer = sections.back();
    convert_chain_thought_to_qa_bean(dst, std::span(sections).subspan(0, sections.size() - 1));
}

static void load_parquet(GSM8kReader::RawDataType &dst, const fs::path &file_path) {
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(
            infile, arrow::io::ReadableFile::Open(file_path.string(), arrow::default_memory_pool()));
    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_THROW_NOT_OK(
            parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));
    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
    SPDLOG_INFO("Loaded {} with {} rows in {} columns.", file_path.filename(), table->num_rows(), table->num_columns());
    dst.clear();
    dst.reserve(table->num_rows());
    for (int64_t i = 0; i < table->num_rows(); i++) {
        auto row = table->Slice(i, 1);
        GSM8kItem sample;
        convert_table_to_qa_bean(sample, *row);
        SPDLOG_INFO("Q: {}", sample.question);
        SPDLOG_INFO("A: {}", sample.answer);
        SPDLOG_INFO("step by step:");
        for (auto &[q, a]: sample.chain_thought_steps) {
            SPDLOG_INFO("q: {}, a: {}", q, a);
        }
        dst.emplace_back(std::move(sample));
    }
}

void GSM8kReader::sync_load_all(std::string_view root_dir) {
    std::array<GSM8kReader::RawDataType *, 4> dst_array = {
            &main_train_data,
            &main_test_data,
            &socratic_train_data,
            &socratic_test_data,
    };
    assert(dst_array.size() == GSM8K_FILES.size());
    for (size_t i = 0; i < dst_array.size(); i++) {
        fs::path file_path = root_dir;
        file_path.append(GSM8K_FILES[i]);
        load_parquet(*dst_array[i], file_path);
    }
}

std::pair<az::pen::PenGraph *, LabelType *> GSM8kReader::next() {
    return {};
}

az::SizeType GSM8kReader::size() const {
    return 0;
}

void GSM8kReader::binary_serialize(std::ostream &out) {
}

void GSM8kReader::binary_deserialize(std::istream &in) {
}
