#include "az/math/alkane_isomer.h"
#include "az/core/utils.h"
#include "boost/iostreams/device/file.hpp"

#include <spdlog/spdlog.h>

#include <taskflow/taskflow.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/unordered/concurrent_flat_set.hpp>

#include <unordered_set>
#include <iomanip>
#include <stack>
#include <vector>
#include <functional>
#include <numeric>
#include <sstream>

using namespace az;
using namespace az::math;

#define PRINT_TIME_COST
//#define XGD_DO_COMPRESSION

#ifdef XGD_DO_COMPRESSION
static const char *DAT_SUFFIX = ".dat.zst";
#else
static const char *DAT_SUFFIX = ".dat";
#endif

namespace az::math::impl {
    template<typename T>
    class InsertOnlySet {
        boost::concurrent_flat_set<T> p;
    public:
        void insert(const T &t) {
            p.insert(t);
        }

        std::vector<T> to_vector() const {
            std::vector<T> out;
            out.reserve(p.size());
            p.visit_all([&](auto &&x) {
                out.emplace_back(x);
            });
            std::sort(out.begin(), out.end());
            return out;
        }

        void visit_all(const std::function<void(const T &)> &on_element) const {
            p.visit_all([&](auto &&x) {
                on_element(x);
            });
        }

        size_t size() const {
            return p.size();
        }

        void reserve(size_t n) { p.reserve(n); }

        void max_load_factor(float z) { p.max_load_factor(z); }
    };

    template<typename T, typename HashType, typename NodeType> requires
    std::is_integral_v<T> && std::is_integral_v<HashType> && std::is_integral_v<NodeType>
    class AlkaneGraph {
        inline static const T EMPTY_NODE = std::numeric_limits<T>::max();
        enum NodeColor {
            WHITE, GRAY, BLACK
        };

        std::vector<NodeColor> state;
        std::vector<std::vector<T>> content;
        using on_node_fn_type = std::function<void(const T &, const T &)>;
        using callback_fn_type = std::function<void(void)>;

        void dfs(
                const T &current,
                const T &from,
                const on_node_fn_type &before,
                const on_node_fn_type &in,
                const on_node_fn_type &after
        ) {
            state[current] = GRAY;
            before(current, from);
            for (auto &neighbor: content[current]) {
                if (state[neighbor] == WHITE) {
                    dfs(neighbor, current, before, in, after);
                    in(neighbor, current);
                }
            }
            after(current, from);
            state[current] = BLACK;
        }


        void dfs_from(
                const T &current,
                const T &from,
                const on_node_fn_type &before,
                const on_node_fn_type &after
        ) {
            before(current, from);
            for (auto &neighbor: content[current]) {
                if (neighbor != from || from == EMPTY_NODE) {
                    dfs_from(neighbor, current, before, after);
                }
            }
            after(current, from);
        }

        std::vector<size_t> max_subtree_size(const size_t &tree_size) {
            std::vector<size_t> subtree_sizes;
            subtree_sizes.reserve(tree_size);
            subtree_sizes.resize(tree_size, 0);

            std::vector<size_t> max_subtree_sizes;
            max_subtree_sizes.reserve(tree_size);
            max_subtree_sizes.resize(tree_size, 0);

            dfs_wrapper([&](const T &current, const T &from) {
                subtree_sizes[current] = 1;
                max_subtree_sizes[current] = 0;
            }, [&](const T &current, const T &from) {
                subtree_sizes[from] += subtree_sizes[current];
                max_subtree_sizes[from] = (std::max)(
                        max_subtree_sizes[from],
                        subtree_sizes[current]
                );
            }, [&](const T &current, const T &from) {
                max_subtree_sizes[current] = (std::max)(
                        max_subtree_sizes[current],
                        tree_size - subtree_sizes[current]
                );
            });
            return max_subtree_sizes;
        }

    public:
        inline static const char PREFIX = '1', SUFFIX = '0';

        std::string to_string() {
            size_t tree_size = content.size();
            std::string final_seq;
            if (tree_size == 1) {
                final_seq += PREFIX;
                final_seq += SUFFIX;
                return final_seq;
            }
            auto max_subtree_sizes = max_subtree_size(tree_size);
            auto min_st = *min_element(max_subtree_sizes.begin(), max_subtree_sizes.end());
            std::vector<std::string> partial_seq(tree_size), full_seq(tree_size);
            for (size_t i = 0; i < tree_size; i++) {
                if (max_subtree_sizes[i] != min_st) {
                    continue;
                }
                dfs_from(i, EMPTY_NODE, [&](const T &current, const T &from) {
                    full_seq[current] = PREFIX;
                }, [&](const T &current, const T &from) {
                    long now = 0;
                    for (auto &neighbor: content[current]) {
                        if (neighbor != from || from == EMPTY_NODE) {
                            partial_seq[now++] = full_seq[neighbor];
                        }
                    }
                    std::sort(partial_seq.begin(), partial_seq.begin() + now);
                    for (long i = 0; i < now; i++) {
                        full_seq[current] += partial_seq[i];
                    }
                    full_seq[current] += SUFFIX;
                });
                if (full_seq[i] > final_seq) {
                    final_seq = full_seq[i];
                }
            }
            return final_seq;
        }

        AlkaneGraph() { content.resize(1); }

        size_t size() const {
            return content.size();
        }

        std::vector<T> &at(T i) {
            return content[i];
        }

        void dfs_wrapper(const on_node_fn_type &before, const on_node_fn_type &in, const on_node_fn_type &after) {
            state.clear();
            state.reserve(content.size());
            state.resize(content.size(), WHITE);
            for (auto i = 0; i < content.size(); i++) {
                if (state[i] == WHITE) {
                    dfs(i, EMPTY_NODE, before, in, after);
                }
            }
            state.clear();
        }

        void push_back(T a, T b) {
            auto max_size = (std::max)(a, b);
            if (content.size() <= max_size) {
                content.resize(max_size + 1);
            }
            content[a].push_back(b);
            content[b].push_back(a);
        }

        void pop_back(T a, T b) {
            content[a].pop_back();
            content[b].pop_back();
            if (content[(std::max)(a, b)].empty()) {
                content.pop_back();
            }
        }

        void add_do_del(T a, T b, const callback_fn_type &callback) {
            push_back(a, b);
            callback();
            pop_back(a, b);
        }

        unsigned long long hash() {
            return std::stoull(to_string(), nullptr, 2);
        }

        static AlkaneGraph<T, HashType, NodeType> GetInstance() {
            return AlkaneGraph<T, HashType, NodeType>();
        }

        void from_hash(const HashType &hash_value) {
            if (hash_value <= 2) {
                *this = GetInstance();
                return;
            }
            std::stringstream ss;
            ss << std::setbase(8) << hash_value;
            std::string hash_string_tmp = std::move(ss.str());
            static std::unordered_map<char, std::string> table = {
                    {'0', "000"},
                    {'1', "001"},
                    {'2', "010"},
                    {'3', "011"},
                    {'4', "100"},
                    {'5', "101"},
                    {'6', "110"},
                    {'7', "111"},
            };
            std::string hash_string;
            for (auto &i: hash_string_tmp) {
                hash_string.append(table[i]);
            }
            std::reverse(hash_string.begin(), hash_string.end());
            while (!hash_string.empty() && hash_string.back() == SUFFIX) {
                hash_string.pop_back();
            }
            // after reverse, hash_string looks like input order
            // std::reverse(hash_string.begin(),hash_string.end());
            // but here i use back() to get char one by one
            // normal: (()) 1100
            // input: 000....0001100
            // now, recover structure from normal: hash_string
            content.clear();
            std::stack<T> node_stack;
            T index = 0;
            while (!hash_string.empty()) {
                char c = hash_string.back();
                hash_string.pop_back();
                if (c == PREFIX) {
                    node_stack.push(index++);
                } else {
                    auto cur = node_stack.top();
                    node_stack.pop();
                    if (!node_stack.empty()) {
                        push_back(node_stack.top(), cur);
                    }
                }
            }
        }
    };
}

using SmiNodeType = unsigned char;
using FastGraph = az::math::impl::AlkaneGraph<SmiNodeType, SmiHashType, SmiNodeType>;
using HashSet = az::math::impl::InsertOnlySet<SmiHashType>;

static void sync_hash_smi_list(
        HashSet &global_set,
        const std::vector<SmiHashType> &last_smi_list,
        size_t begin,
        size_t end
) {
    tf::Taskflow taskflow;
    const auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 192u);
    taskflow.for_each_index(begin, end, size_t{1}, [&](size_t i) {
        FastGraph mol;
        mol.from_hash(last_smi_list[i]);
        size_t total = mol.size();
        for (size_t j = 0; j < total; j++) {
            auto &node = mol.at(j);
            if (node.size() < 4) {
                mol.add_do_del(j, total, [&]() {
                    auto smi = mol.hash();
                    global_set.insert(smi);
                });
            }
        }
    });
    tf::Executor(thread_num).run(taskflow).get();
}

static std::vector<SmiHashType> get_next_isomers(const std::vector<SmiHashType> &last_smi_list) {
    HashSet global_set;
    sync_hash_smi_list(global_set, last_smi_list, 0, last_smi_list.size());
    return global_set.to_vector();
}

std::vector<SmiHashType> AlkaneIsomerUtil::get_isomers_sync(int8_t count) {
    if (count <= 0) {
        throw std::runtime_error(fmt::format("invalid alkane carbon num: {}", count));
    } else if (count == 1) {
        FastGraph g;
        return {g.hash()};
    } else if (count == 2) {
        FastGraph g;
        g.push_back(0, 1);
        return {g.hash()};
    }
    std::vector<SmiHashType> smi_list;
    std::vector<SmiHashType> last_smi_list = {FastGraph::GetInstance().hash()};
    for (int8_t i = 2; i <= count; i++) {
#ifdef PRINT_TIME_COST
        const auto beg = std::chrono::high_resolution_clock::now();
#endif
        smi_list = get_next_isomers(last_smi_list);
#ifdef PRINT_TIME_COST
        const auto end = std::chrono::high_resolution_clock::now();
        SPDLOG_INFO("({}) cost {:.3f} ms", i,
                    std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count() / 1000.f);
#endif
        last_smi_list = std::move(smi_list);
        smi_list = {};
    }
    return last_smi_list;
}

void AlkaneIsomerUtil::dump_isomers_sync(int8_t count, std::string_view path, size_t thread_num) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(fmt::format("{} is not an existing directory", path));
    }
    int8_t start = 0;
    std::string directory{path};
    if (!directory.ends_with(fs::path::preferred_separator)) {
        directory += fs::path::preferred_separator;
    }
    SPDLOG_INFO("directory={}", directory);
    while (fs::exists(directory + fmt::format("{}{}", start + 1, DAT_SUFFIX))) {
        start++;
    }

    std::vector<SmiHashType> smi_chunk;

    namespace bi = boost::iostreams;
    bi::filtering_istream in;
    bi::filtering_ostream out;
#ifdef XGD_DO_COMPRESSION
    in.push(bi::zstd_decompressor());
    out.push(bi::zstd_compressor(bi::zstd_params(bi::zstd::best_compression)));
#endif
    if (start == 0) {
        FastGraph g;
        smi_chunk = {g.hash()};
        std::ofstream out_stream(
                directory + fmt::format("{}{}", 1, DAT_SUFFIX), std::ios_base::out | std::ios_base::binary);
        out.push(out_stream);
        size_t isomer_count = smi_chunk.size();
        out.write(reinterpret_cast<char *>(&isomer_count), sizeof(size_t));
        out.write(reinterpret_cast<const char *>(smi_chunk.data()), sizeof(SmiHashType) * isomer_count);
        out.pop();
        start++;
    }
    static const std::array<size_t, 33> ground_truth = {
            0, // skip c-0
            1, 1, 1, 2,
            3, 5, 9, 18,
            35, 75, 159, 355,
            802, 1858, 4347, 10359,
            24894, 60523, 148284, 366319,
            910726, 2278658, 5731580, 14490245,
            36797588, 93839412, 240215803, 617105614,
            1590507121, 4111846763, 10660307791, 27711253769,
    };
    // now start+1 point to a missing dat file
    for (int8_t n = start + 1; n <= count; n++) {
        in.push(bi::file_source(
                directory + fmt::format("{}{}", n - 1, DAT_SUFFIX)), std::ios_base::in | std::ios_base::binary);
        size_t last_count = 0;
        in.read(reinterpret_cast<char *>(&last_count), sizeof(size_t));

        HashSet global_set;
        const float factor = 2.6;
        global_set.reserve(last_count * factor);
        global_set.max_load_factor(factor); // actually not work for flat map
        const size_t chunk_max_size = 1024 * 1024 * 100; // 100M
        for (size_t k = 0; k < std::ceil(1. * last_count / chunk_max_size); k++) {
            size_t chunk_begin = k * chunk_max_size;
            if (global_set.size() == ground_truth[n]) {
                SPDLOG_INFO("early stop by ratio {}", 1.0 * chunk_begin / last_count);
                break;
            }
            size_t chunk_end = (std::min)((k + 1) * chunk_max_size, last_count);
            size_t chunk_size = chunk_end - chunk_begin;
            smi_chunk.reserve(chunk_size);
            smi_chunk.resize(chunk_size);
            in.read(reinterpret_cast<char *>(smi_chunk.data()), sizeof(SmiHashType) * chunk_size);

            tf::Taskflow taskflow;
            if (0 == thread_num) {
                thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 192u);
            }
            taskflow.for_each_index(size_t{0}, smi_chunk.size(), size_t{1}, [&](size_t i) {
                FastGraph mol;
                mol.from_hash(smi_chunk[i]);
                size_t total = mol.size();
                for (size_t j = 0; j < total; j++) {
                    auto &node = mol.at(j);
                    if (node.size() < 4) {
                        mol.add_do_del(j, total, [&]() {
                            auto smi = mol.hash();
                            global_set.insert(smi);
                        });
                    }
                }
            });
            tf::Executor(thread_num).run(taskflow).get();
        }
        in.pop();

        std::ofstream out_stream(
                directory + fmt::format("{}{}", n, DAT_SUFFIX), std::ios_base::out | std::ios_base::binary);
        out.push(out_stream);
        size_t isomer_count = global_set.size();
        SPDLOG_INFO("alkanes {} count {}", n, isomer_count);
        out.write(reinterpret_cast<char *>(&isomer_count), sizeof(size_t));
        global_set.visit_all([&out](auto &&x) {
            out.write(reinterpret_cast<const char *>(&x), sizeof(SmiHashType) * 1);
        });
        out.pop();
        SPDLOG_INFO("alkanes {} generation done", n);
    }
}

LabelType AlkaneIsomerUtil::hash_to_smi(SmiHashType smi_hash) {
    FastGraph alkane;
    alkane.from_hash(smi_hash);
    auto hash_str = alkane.to_string();
    std::string smiles;
    for (auto &c: hash_str) {
        if (c == FastGraph::PREFIX) {
            smiles.append("(C");
        } else {
            smiles.push_back(')');
        }
    }
    if (smiles.size() > 2 && smiles.front() == '(' && smiles.back() == ')') {
        smiles = smiles.substr(1, smiles.size() - 2);
    }
    return smiles;
}

std::vector<LabelType> AlkaneIsomerUtil::hash_to_smi_batched(const std::vector<SmiHashType> &smi_hash_list) {
    size_t total = smi_hash_list.size();
    std::vector<LabelType> result;
    result.reserve(total);
    result.resize(total);
    tf::Taskflow taskflow;
    const auto thread_num = std::clamp(std::thread::hardware_concurrency(), 1u, 16u);
    taskflow.for_each_index(size_t{0}, total, size_t{1}, [&](size_t i) {
        result[i] = AlkaneIsomerUtil::hash_to_smi(smi_hash_list[i]);
    });
    tf::Executor(thread_num).run(taskflow).get();
    return result;
}
