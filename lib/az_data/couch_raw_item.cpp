#include "couch_raw_item.h"

#include <boost/locale/encoding.hpp>
#include <spdlog/spdlog.h>

using namespace az;
using namespace az::data;
using namespace az::data::impl;

static std::string GBKToUTF8(std::string_view strGBK) {
    return boost::locale::conv::to_utf<char>(strGBK.data(), "gb18030", boost::locale::conv::stop);
}

PointerVec<CouchItem> CouchRawItem::read(std::istream &idx_in, std::istream &dat_in) {
    PointerVec<CouchItem> data;
    istream_read_buffer(idx_in, &sample_num, 4);
    while (istream_read_buffer(idx_in, &sample_state, 1)) {
        istream_read_buffer(idx_in, &osw_index, 4);
        istream_read_buffer(idx_in, &idx_index, 4);
        istream_read_buffer(idx_in, &dat_offset, 4);
        dat_in.seekg(dat_offset, std::ios::beg);
        istream_read_buffer(dat_in, &word_length, 1);
        word_code.resize(1 * (size_t) word_length + 1);
        istream_read_buffer(dat_in, word_code.data(), 1 * word_length);
        word_code.back() = '\0';
        istream_read_buffer(dat_in, &point_num, 2);
        istream_read_buffer(dat_in, &line_num, 2);
        istream_read_buffer(dat_in, &get_time_point_num, 2);
        get_time_point_index.resize(get_time_point_num);
        elapsed_time.resize(4 * (size_t) get_time_point_num);
        istream_read_buffer(dat_in, get_time_point_index.data(), 2 * get_time_point_num);
        istream_read_buffer(dat_in, elapsed_time.data(), 4 * get_time_point_num);

        auto item = std::make_shared<CouchItem>();
        try {
            item->label = GBKToUTF8(word_code.data());
        } catch (std::exception &e) {
            SPDLOG_ERROR("failed to convert {}. Exception: {}", word_code.data(), e.what());
            throw e;
        }
        item->read(dat_in, line_num);
        data.push_back(item);
    }
    return data;
}