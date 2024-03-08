#pragma once

#include "az/core/utils.h"
#include "az/pen/pen_string.h"

#include "az/data/config.h"
#include "az/data/inkml_item.h"

#include <unordered_map>

namespace az::data {
    /**
     *
     * unzip latest WebData_CROHME23.zip
     *
     * DO NOT USE dataset download from zenodo. Too many mistakes like raw "<" in xml.
     *      (x) https://zenodo.org/records/8428035/files/CROHME23.zip
     */
    class AZDATA_EXPORT InkmlDatasetReader : public DataSetReader<az::pen::PenString, LabelType> {
        static std::vector<std::string_view> HME23_SUB_DIRS;
    public:
        using DataSourceType = std::vector<InkmlItem>;

        void sync_load_all(
                std::string_view root_dir,
                const std::vector<std::string_view> &sub_dirs = HME23_SUB_DIRS
        );

        az::pen::PenChar *select(const LabelType &label);

        std::pair<az::pen::PenString *, LabelType *> next() final;

        SizeType size() const final;

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

        void get_keys(const std::function<void(const LabelType &)> &on_key) const;

    private:
        DataSourceType data_source;
        // <unicode, [char level item]>
        std::unordered_map<LabelType, std::vector<az::pen::PenChar>> single_item_source;
    };
}
