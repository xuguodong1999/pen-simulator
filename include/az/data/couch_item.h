#pragma once

#include "az/data/config.h"
#include "az/core/utils.h"
#include "az/pen/pen_char.h"

namespace az::data {
    struct AZDATA_EXPORT CouchItem {
        UCharType label;
        az::pen::PenChar data;

        void read(std::istream &in, IndexType stroke_num);
    };
}
