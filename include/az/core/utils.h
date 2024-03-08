#pragma once

#include "az/core/config.h"

#include <istream>
#include <vector>
#include <filesystem>
#include <random>
#include <numeric>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace az {
    namespace fs = std::filesystem;

    template<typename K, typename V>
    void insert_unordered_map_vector(
            std::unordered_map<K, std::vector<std::decay_t<V>>> &container,
            const K &k,
            V &&v
    ) {
        auto it = container.find(k);
        if (it == container.end()) {
            bool ok;
            std::tie(it, ok) = container.insert({k, std::vector<std::decay_t<V>>{}});
            if (!ok) {
                throw std::runtime_error("insertion failed");
            }
        }
        it->second.push_back(std::forward<std::decay_t<V>>(v));
//        if (!container.contains(k)) {
//            container[k] = std::vector<std::decay_t<V>>{};
//        }
//        container[k].push_back(std::forward<std::decay_t<V>>(v));
    }

    // https://stackoverflow.com/questions/42519867/efficiently-moving-contents-of-stdunordered-set-to-stdvector
    template<typename T>
    std::vector<T> move_unordered_set_to_vector(std::unordered_set<T> &set) {
        std::vector<T> result;
        result.reserve(set.size());
        for (auto it = set.begin(); it != set.end();) {
            result.push_back(std::move(set.extract(it++).value()));
        }
        return result;
    }

    template<typename T>
    void merge_unordered_set_to_another(std::unordered_set<T> &src, std::unordered_set<T> &dst) {
        for (auto it = src.begin(); it != src.end();) {
            dst.insert(std::move(src.extract(it++).value()));
        }
    }

    template<typename T>
    auto &istream_read_buffer(std::istream &ism, T *var, std::streamsize length) {
        return ism.read(reinterpret_cast<char *>(var), length);
    }

    template<typename T>
    void move_append(std::vector<T> &dst, std::vector<T> &src) {
        if (dst.empty()) {
            dst = std::move(src);
        } else {
            auto dst_size = dst.size() + src.size();
            if (dst.capacity() < dst_size) {
                dst.reserve(dst_size);
            }
            std::move(std::begin(src), std::end(src), std::back_inserter(dst));
        }
    }

    struct AZCORE_EXPORT BinaryIO {
        virtual void binary_serialize(std::ostream &out) { throw std::runtime_error("Not Implemented"); }

        virtual void binary_deserialize(std::istream &in) { throw std::runtime_error("Not Implemented"); }
    };

    struct AZCORE_EXPORT TextIO {
        virtual void serialize(std::ostream &out) { throw std::runtime_error("Not Implemented"); }

        virtual void deserialize(std::istream &in) { throw std::runtime_error("Not Implemented"); }
    };

    template<class X, class Y>
    struct DataSetReader : public BinaryIO, public TextIO {
        virtual std::pair<X *, Y *> next() = 0;

        virtual SizeType size() const = 0;
    };

    AZCORE_EXPORT std::string erase_char_from_string(std::string_view sv, char exclude);

    AZCORE_EXPORT std::string get_dataset_root(const std::string &file_name);

    AZCORE_EXPORT std::string get_tmp_root(const std::string &file_name);

    AZCORE_EXPORT std::random_device &get_random_device();

    AZCORE_EXPORT std::vector<fs::path> scan_files_by_ext(const fs::path &path, const char *ext);

    template<typename T>
    T &random_select(std::vector<T> &bucket) {
        std::mt19937 rng(get_random_device()());
        std::uniform_int_distribution<size_t> dist_buckets(0, bucket.size() - 1);
        return bucket[dist_buckets(rng)];
    }

    template<typename T>
    requires std::is_floating_point_v<T>
    T random_float(const T &min_v, const T &max_v) {
        std::mt19937 rng(get_random_device()());
        std::uniform_real_distribution<T> dist(min_v, max_v);
        return dist(rng);
    }

    /**
     * @return [min_v , max_v]
     */
    template<typename T>
    requires std::is_integral_v<T>
    T random_int(const T &min_v, const T &max_v) {
        std::mt19937 rng(get_random_device()());
        std::uniform_int_distribution<T> dist(min_v, max_v);
        return dist(rng);
    }

    template<typename FloatType, typename T>
    FloatType vector_mean(const std::vector<T> &src) {
        return std::reduce(src.begin(), src.end(), T{0}, [n = FloatType{0}](auto cma, auto i) mutable -> FloatType {
            return cma + (i - cma) / ++n;
        });
    }

    template<typename T>
    T vector_medium(const std::vector<T> &src) {
        std::priority_queue<T> g, s;
        for (size_t i = 0; i < src.size(); i++) {
            s.push(src[i]);
            T temp = s.top();
            s.pop();
            g.push(-1 * temp);
            if (g.size() > s.size()) {
                temp = g.top();
                g.pop();
                s.push(-1 * temp);
            }
        }
        return s.top();
    }
}
