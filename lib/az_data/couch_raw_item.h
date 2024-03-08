#pragma once

#include "az/data/config.h"
#include "az/data/couch_item.h"

#include <cstdint>
#include <vector>
#include <istream>

namespace az::data::impl {
    struct CouchRawItem {
        int32_t sample_num;                          // 4
        unsigned char sample_state;                  // 1
        int32_t osw_index;                           // 4
        int32_t idx_index;                           // 4
        int32_t dat_offset;                          // 4
        unsigned char word_length;                   // 1
        std::vector<char> word_code;                 // 1*WordLength
        uint16_t point_num;                          // 2
        uint16_t line_num;                           // 2
        uint16_t get_time_point_num;                 // 2
        std::vector<uint16_t> get_time_point_index;  // 2*getTimePointNum
        std::vector<int32_t> elapsed_time;           // 4*getTimePointNum

        PointerVec<CouchItem> read(std::istream &idx_in, std::istream &dat_in);
    };
}