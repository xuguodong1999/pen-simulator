#pragma once

#include "az/data/config.h"
#include "az/core/utils.h"
#include "az/pen/pen_char.h"
#include "az/pen/pen_graph.h"

#include <unordered_map>

namespace az::data {
    class AZDATA_EXPORT UnicodeSvgReader : public DataSetReader<az::pen::PenGraph, UCharType> {
    public:
        using DataSourceType = std::unordered_map<UCharType, PointerVec<az::pen::PenGraph>>;

        void sync_load_all(std::string_view graphics_txt_path = "graphics.txt");

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

        az::pen::PenGraph *select(const UCharType &label);

        std::pair<az::pen::PenGraph *, UCharType *> next() final;

        SizeType size() const final;

        const std::vector<UCharType> &get_keys() const;

    private:
        DataSourceType data_source;
        std::vector<UCharType> keys;
    };
}
