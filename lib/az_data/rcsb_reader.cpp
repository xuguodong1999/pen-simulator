#include "az/data/rcsb_reader.h"
#include "az/chem/mol_graph.h"
#include "az/chem/utils.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/array.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <taskflow/taskflow.hpp>

using namespace az;
using namespace az::data;
using namespace az::chem;

static void gunzip(std::string &out, const std::string &in) {
    namespace bi = boost::iostreams;
    bi::filtering_ostream osm;
    osm.push(bi::gzip_decompressor());
    osm.push(bi::back_inserter(out));
    osm.write(in.data(), in.size());
    bi::close(osm);
}

static void read_gz_batched(
        const std::vector<fs::path> &path_list,
        const std::function<void(const fs::path &, const std::string &)> on_content
) {
    tf::Taskflow taskflow;
    const auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 192u);
    taskflow.for_each_index(size_t{0}, path_list.size(), size_t{1}, [&](size_t i) {
        const fs::path &file_info = path_list[i];
        std::ifstream file(file_info, std::ios_base::in);
        std::string gzipped_buffer;
        istream_read_all(gzipped_buffer, file);
        std::string buffer;
        gunzip(buffer, gzipped_buffer);
        on_content(file_info, buffer);
    });
    const auto beg = std::chrono::high_resolution_clock::now();
    tf::Executor(thread_num).run(taskflow).get();
    const auto end = std::chrono::high_resolution_clock::now();
    SPDLOG_INFO("use {} threads cost {:.3f} ms", thread_num,
                std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count() / 1000.f);
}

void RCSBReader::sync_load_all(std::string_view root_dir, const std::string_view &sub_dir) {
    fs::path root = root_dir;
    root += fs::path::preferred_separator;
    std::vector<fs::path> files;
    {
        fs::path dir = root;
        dir += sub_dir;
        std::vector<fs::path> sub_dir_files = scan_files_by_ext(dir, ".gz");
        SPDLOG_INFO("{} contain {} gz files", dir.string(), sub_dir_files.size());
        move_append(files, sub_dir_files);
    }
    std::mutex mutex;
    read_gz_batched(files, [&](auto &&f, auto &&x) {
        std::string out;
        try {
            out = az::chem::Utils::convert_format(x, "mmCIF", "inchi");
            SPDLOG_INFO("{}={}", f.filename(), out);
        } catch (std::exception &e) {
            SPDLOG_ERROR("{}: {}", f, e.what());
        }
        {
            std::unique_lock lk(mutex);
        }
    });
}

std::pair<az::chem::MolGraph *, az::LabelType *> RCSBReader::next() {
    return {};
}

az::SizeType RCSBReader::size() const {
    return 0;
}

void RCSBReader::binary_serialize(std::ostream &out) {
}

void RCSBReader::binary_deserialize(std::istream &in) {
}
