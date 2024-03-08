#pragma once

#include "az/data/config.h"
#include "az/data/couch_item.h"
#include "az/core/utils.h"
#include "az/pen/pen_char.h"

#include <unordered_map>

namespace az::data {
    class AZDATA_EXPORT CouchDatasetReader : public DataSetReader<az::pen::PenChar, UCharType> {
        static std::vector<std::string_view> COUCH_SUB_DIRS;
    public:
        using DataSourceType = std::unordered_map<UCharType, PointerVec<CouchItem>>;

        void sync_load_all(
                std::string_view couch_dir,
                const std::vector<std::string_view> &sub_dirs = COUCH_SUB_DIRS
        );

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

        az::pen::PenChar *select(const UCharType &label);

        std::pair<az::pen::PenChar *, UCharType *> next() final;

        SizeType size() const final;

        const std::vector<UCharType> &get_keys() const;

    private:
        DataSourceType data_source;
        std::vector<UCharType> keys;
    };
}
