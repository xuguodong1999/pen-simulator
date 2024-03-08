#pragma once

#include "az/core/utils.h"
#include "az/pen/pen_string.h"

#include "az/data/config.h"
#include "az/data/inkml_item.h"

#include <unordered_map>

namespace az::data {
    /**
     * https://zenodo.org/records/56198
     * clean with www/playground/mathjax-fork-jit/src/clean-im2latex.ts
     * got im2latex_formulas.lst.json, structured as {
     *  "valid":["s1","s2",...],
     *  "invalid":["s1","s2",...],
     * }
     */
    class AZDATA_EXPORT Im2LatexReader : public DataSetReader<LabelType, int8_t> {
    public:
        using DataSourceType = std::vector<LabelType>;
        using DataLabelType = std::vector<int8_t>;

        void sync_load_all(std::string_view file_path = "im2latex_formulas.lst.json");

        std::pair<LabelType *, int8_t *> next() final;

        SizeType size() const final;

        void binary_serialize(std::ostream &out) override;

        void binary_deserialize(std::istream &in) override;

    private:
        DataSourceType data_source;
        DataLabelType label_source;
    };
}
