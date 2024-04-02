#pragma once

#include "az/core/utils.h"

#include "az/data/config.h"

namespace az::pen {
    struct PenGraph;
}

namespace az::data {
    struct AZDATA_EXPORT GSM8kItem {
        std::string question;
        std::string answer;
        std::vector<std::pair<std::string, std::string>> chain_thought_steps;
    };
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
    class AZDATA_EXPORT GSM8kReader : public DataSetReader<az::pen::PenGraph, LabelType> {
        static std::vector<std::string_view> GSM8K_FILES;
    public:
        using DataSourceType = PointerVec<az::pen::PenGraph>;
        using RawDataType = std::vector<GSM8kItem>;

        void sync_load_all(std::string_view root_dir);

        std::pair<az::pen::PenGraph *, LabelType *> next() final;

        SizeType size() const final;

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

    private:
        DataSourceType data_source;
        RawDataType socratic_train_data;
        RawDataType socratic_test_data;
        RawDataType main_train_data;
        RawDataType main_test_data;
    };
}
