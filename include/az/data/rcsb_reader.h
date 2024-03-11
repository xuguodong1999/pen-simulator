#pragma once

#include "az/core/utils.h"

#include "az/data/config.h"

#include <unordered_map>

namespace az::chem {
    class MolGraph;
}

namespace az::data {
    /**
     * work in progress...
     * rcsb
        ├── mmCIF
        └── pdb
     */
    class AZDATA_EXPORT RCSBReader : public DataSetReader<az::chem::MolGraph, LabelType> {
    public:
        using DataSourceType = PointerVec<az::chem::MolGraph>;

        void sync_load_all(
                std::string_view root_dir,
                const std::string_view &sub_dir = "mmCIF"
        );

        std::pair<az::chem::MolGraph *, LabelType *> next() final;

        SizeType size() const final;

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

    private:
        DataSourceType data_source;
    };
}
