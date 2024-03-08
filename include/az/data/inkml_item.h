#pragma once

#include "az/data/config.h"
#include "az/core/utils.h"
#include "az/pen/pen_string.h"

namespace az::data {
    struct AZDATA_EXPORT InkmlItem {
        LabelType latex;

        az::pen::PenString data;
    };
}