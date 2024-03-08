#include "az/math/multiply_draft.h"
#include "az/core/utils.h"
#include <spdlog/spdlog.h>

#include <boost/algorithm/string/erase.hpp>

#include <iterator>
#include <ranges>
#include <unordered_set>

using namespace az;
using namespace az::math;

static const char DOT_CHAR = '.';
static const char ZERO_CHAR = '0';
static const std::string DOT_STR(1, DOT_CHAR);
static const std::string NUMBER_HOLDER = R"(\phantom{0})";
static const std::string DOT_HOLDER = R"(\phantom{.})";

static bool is_non_prefix(char c) {
    return c == ZERO_CHAR || c == DOT_CHAR;
}

static int64_t count_significant_digits(std::string_view number) {
    if (number.empty()) { return 0; }
    int64_t beg = 0;
    int64_t end = number.length() - 1;
    while (beg < number.length() && is_non_prefix(number[beg])) { beg++; }
    while (end >= 0 && is_non_prefix(number[end])) { end--; }
    return end - beg + 1 - std::ranges::count(number, DOT_CHAR);
}

static void filter_input(std::string &input) {
    size_t dot_count = std::ranges::count(input, DOT_CHAR);
    if (dot_count > 1) {
        throw std::runtime_error(fmt::format("unexpected {} with more than one dot", input));
    } else if (dot_count == 1) {
        while (input.ends_with(ZERO_CHAR)) {
            input.pop_back();
        }
        if (input.starts_with(DOT_STR)) {
            input = "0" + input;
        } else if (input.ends_with(DOT_STR)) {
            input.pop_back();
        }
    }
}

// TODO: cleanup messy codes
static std::string _make_number_line(
        std::string_view line,
        const std::unordered_set<size_t> &point_offsets,
        size_t total_offset,
        size_t offset1,
        size_t offset2,
        const int suffix_length = 0) {
    std::string out;
    if (line.empty()) { return out; }
    {
        size_t i = 0;
        while (i < line.length() && line[i] == ZERO_CHAR) { i++; }
        if (i == line.length()) { return out; }
    }
    size_t i = 0;
    while (line[i] == ZERO_CHAR) {
        out += NUMBER_HOLDER;
        i++;
    }
    if (line[i] == DOT_CHAR) {
        out += ZERO_CHAR;
    }
    for (; i < line.length(); i++) {
        out += line[i];
        if (line[i] == DOT_CHAR) { continue; }
        // 5+0-0-1=4
        int64_t offset = line.length() + suffix_length - i - 1;
        if (offset != 0 && suffix_length >= 0 && point_offsets.contains(offset)) {
            out += DOT_HOLDER;
        } else if (suffix_length == -1) { // input line 1
            offset = line.length() - i - 1;
            if (offset2 > 0 && offset == offset2 + (offset1 == 0 ? 0 : (offset2 > offset1))) {
                out += DOT_HOLDER;
            } else if (total_offset > 0 && offset == total_offset + (offset1 == 0 ? 0 : (total_offset > offset1))) {
                out += DOT_HOLDER;
            }
        } else if (suffix_length == -2) { // input line 2
            offset = line.length() - i - 1;
            if (offset1 > 0 && offset == offset1 + (offset2 == 0 ? 0 : (offset1 > offset2))) {
                out += DOT_HOLDER;
            } else if (total_offset > 0 && offset == total_offset + (offset1 == 0 ? 0 : (total_offset > offset1))) {
                out += DOT_HOLDER;
            }
        }
    }
    return out;
}

LabelType MultiplyDraft::generate(
        std::string_view i1,
        std::string_view i2,
        std::vector<LabelType> *dst_lines
) {
    // generation direction is opposite to storage direction
    std::string input_1 = i1.data();
    std::string input_2 = i2.data();
    if (count_significant_digits(input_1) < count_significant_digits(input_2)) {
        std::swap(input_1, input_2);
    }
    filter_input(input_1);
    filter_input(input_2);
    std::vector<std::string> mid_results;
    std::string final_result;
    std::unordered_set<size_t> point_offsets;
    size_t total_offset = 0;
    size_t offset1 = 0;
    size_t offset2 = 0;
    if (auto it = input_1.find(DOT_CHAR); it != std::string::npos) {
        offset1 = input_1.length() - it - 1;
        total_offset += offset1;
        point_offsets.insert(offset1);
    }
    if (auto it = input_2.find(DOT_CHAR); it != std::string::npos) {
        offset2 = input_2.length() - it - 1;
        total_offset += offset2;
        point_offsets.insert(offset2);
    }
    point_offsets.insert(total_offset);
    const auto make_number_line = [&](std::string_view line, const int suffix_length = 0) {
        return _make_number_line(line, point_offsets, total_offset, offset1, offset2, suffix_length);
    };
    const auto on_line_break = [](std::string &out) {
        out += '\n';
    };
    const auto on_new_content_line = [](std::string &out) {
        out += R"(&\\)";
        out += '\n';
    };
    const auto on_new_split_line = [](std::string &out) {
        out += R"(\hline)";
        out += '\n';
    };
    // two pure number input line
    std::string line_1 = erase_char_from_string(input_1, DOT_CHAR); // 1234
    std::string line_2 = erase_char_from_string(input_2, DOT_CHAR); // 567
    std::string line_3 = std::to_string(std::stoi(line_1) * std::stoi(line_2));
    // calculate middle result by multiple
    for (const char &it2: std::ranges::reverse_view(line_2)) {
        const auto n2 = it2 - ZERO_CHAR;
        int forward = 0;
        std::string new_line;
        for (const char &it1: std::ranges::reverse_view(line_1)) {
            const auto n1 = it1 - ZERO_CHAR;
            int result = n1 * n2 + forward;
            int current = result % 10;
            forward = std::floor((result - current) / 10);
            new_line += char(current + ZERO_CHAR);
        }
        if (forward > 0) {
            new_line += std::to_string(forward);
        }
        std::reverse(new_line.begin(), new_line.end());
        mid_results.push_back(new_line);
    }
    size_t max_line_size = 0;
    for (size_t i = 0; i < mid_results.size(); i++) {
        max_line_size = (std::max)(max_line_size, i + mid_results[i].length());
    }
    // calculate final result ny add
    int forward = 0;
    for (size_t i = 0; i < max_line_size; i++) {
        int result = forward;
        for (size_t j = 0; j < mid_results.size() && j <= i; j++) {
            size_t total = mid_results[j].length();
            int offset = total - (i - j) - 1;
            if (offset < 0) { continue; }
            result += mid_results[j][offset] - ZERO_CHAR;
        }
        int current = result % 10;
        forward = std::floor((result - current) / 10);
        final_result += char(current + ZERO_CHAR);
    }
    if (forward > 0) {
        final_result += std::to_string(forward);
    }
    std::reverse(final_result.begin(), final_result.end());

    // make latex with phantom
    std::string latex = R"(\begin{align})";
    on_line_break(latex);
    { // input line 1
        std::string new_line = make_number_line(input_1, -1);
        if (new_line.empty()) { new_line += ZERO_CHAR; }
        latex += new_line;
        on_new_content_line(latex);
        if (dst_lines) { dst_lines->push_back(input_1); }
    }
    { // input line 2
        latex += R"(\times)";
        for (size_t i = 0; i < line_3.length() - line_2.length() + 1; i++) {
            latex += NUMBER_HOLDER;
        }
        for (size_t i = 0; i < point_offsets.size() - 1; i++) {
            latex += DOT_HOLDER;
        }
        std::string new_line = make_number_line(input_2, -2);
        if (new_line.empty()) { new_line += ZERO_CHAR; }
        latex += new_line;
        on_new_content_line(latex);
        on_new_split_line(latex);
        if (dst_lines) {
            dst_lines->push_back("×" + input_2);
            dst_lines->push_back("-");
        }
    }
    // middle results
    bool has_mid_line = false;
    for (size_t i = 0; i < mid_results.size(); i++) {
        const auto &mid_result = mid_results[i];
        auto new_line = make_number_line(mid_result, i);
        if (new_line.empty()) {
            continue;
        }
//        if (i == mid_results.size() - 1) {
//            latex += '+';
//            latex += NUMBER_HOLDER;
//        }
        if (dst_lines) { dst_lines->push_back(mid_result); }
        has_mid_line = true;
        latex += new_line;
        for (int j = 0; j < i; j++) {
            latex += NUMBER_HOLDER;
            if (j != 0 && point_offsets.contains(j)) {
                latex += DOT_HOLDER;
            }
        }
        on_new_content_line(latex);
    }
    // line break
    if (has_mid_line) {
        on_new_split_line(latex);
        if (dst_lines) { dst_lines->push_back("-"); }
    }
    // result
    int64_t dot_offset = final_result.length() - offset1 - offset2;
    if (dot_offset < final_result.length()) {
        final_result.insert(dot_offset, DOT_STR);
    }
    if (dst_lines) { dst_lines->push_back(final_result); }
    latex += make_number_line(final_result);
    on_line_break(latex);

    latex += R"(\end{align})";
    return latex;
}
