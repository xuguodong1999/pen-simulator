#include "az/core/utils.h"

#include <algorithm>

using namespace az;

std::vector<fs::path> az::scan_files_by_ext(const fs::path &path, const char *ext) {
    std::vector<fs::path> files;
    for (auto &p: fs::recursive_directory_iterator(path)) {
        if (p.is_regular_file() && p.path().extension() == ext) {
            files.push_back(p);
        }
    }
    return files;
}

void az::istream_read_all(std::string &buffer, std::istream &in) {
    in.seekg(0, std::ios::beg);
    std::streamsize size = in.tellg();
    using It = std::istreambuf_iterator<char>;
    buffer.reserve(size);
    buffer.insert(buffer.begin(), It(in), It());
}

std::random_device &az::get_random_device() {
    static std::random_device dev;
    return dev;
}

std::string az::erase_char_from_string(std::string_view sv, char exclude) {
    std::string out;
    std::copy_if(sv.begin(), sv.end(), std::back_inserter(out), [&](char c) {
        return c != exclude;
    });
    return out;
}

std::string az::get_dataset_root(const std::string &file_name) {
    std::string root;
#ifdef _WIN32
    root = R"(D:\datasets\ntfs\)";
#elif __linux__
    root = R"(/home/xgd/datasets/)";
#else
    root = "./";
#endif
    return root + file_name;
}

std::string az::get_tmp_root(const std::string &file_name) {
    std::string root;
#ifdef _WIN32
    root = R"(D:\TEMP\1\)";
#elif __linux__
    root = R"(/tmp/1/)";
//    root = R"(/home/xgd/datasets/cache/frames/)";
//    root = R"(/mnt/d/TEMP/1/)";
#else
    root = "./";
#endif
    return root + file_name;
}
