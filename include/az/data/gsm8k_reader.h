#pragma once

#include "az/core/utils.h"

#include "az/data/config.h"

#include <unordered_map>

namespace az::pen {
    struct PenGraph;
}

namespace az::data {
    /**
     * gsm8k
        ├── main
        │     ├── test-00000-of-00001.parquet
        │     └── train-00000-of-00001.parquet
        ├── README.md
        └── socratic
            ├── test-00000-of-00001.parquet
            └── train-00000-of-00001.parquet
     */
    class AZDATA_EXPORT Gsm8kReader : public DataSetReader<az::pen::PenGraph, LabelType> {
    public:
        using DataSourceType = PointerVec<az::pen::PenGraph>;

        void sync_load_all(std::string_view root_dir);

        std::pair<az::pen::PenGraph *, LabelType *> next() final;

        SizeType size() const final;

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

    private:
        DataSourceType data_source;
    };
}
