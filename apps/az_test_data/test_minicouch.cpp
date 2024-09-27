#include "az/data/couch_reader.h"

#include <spdlog/spdlog.h>
#include <boost/locale/encoding.hpp>
#include <boost/math/constants/constants.hpp>

#include <gtest/gtest.h>

#include <fstream>
#include <algorithm>

template<typename T>
static auto &ostream_write_buffer(std::ostream &osm, T *var, std::streamsize length) {
    return osm.write(reinterpret_cast<const char *>(var), length);
}

static std::string UTF8ToGBK(std::string_view strUTF8) {
    return boost::locale::conv::between(strUTF8.data(), "gb18030", "utf8", boost::locale::conv::stop);
}

static std::string GBKToUTF8(std::string_view strGBK) {
    return boost::locale::conv::to_utf<char>(strGBK.data(), "gb18030", boost::locale::conv::stop);
}

static az::pen::PenStroke shrink_stroke(az::pen::PenStroke &stroke) {
    if (stroke.count() < 5) { return stroke; }
    az::Vec2Array old_pts = stroke.as_points();
    az::Vec2Array new_pts;
    new_pts.push_back(old_pts.front());
    bool last_skipped = false;
    for (size_t i = 0; i < stroke.count() - 3; i++) {
        auto &p1 = old_pts[i];
        auto &p0 = old_pts[i + 1];
        auto &p2 = old_pts[i + 2];
        if (last_skipped) {
            last_skipped = false;
            new_pts.push_back(p0);
            continue;
        }
        // https://stackoverflow.com/questions/1211212/how-to-calculate-an-angle-from-three-points/31334882#31334882
        double angle = std::abs(atan2(p2.y() - p0.y(), p2.x() - p0.x()) - atan2(p1.y() - p0.y(), p1.x() - p0.x()))
                       * 180 / boost::math::constants::pi<az::ScalarType>();
        if (135 < angle && angle <= 180) {
            last_skipped = true;
        } else {
            new_pts.push_back(p0);
        }
    }
    new_pts.push_back(old_pts.back());
    az::pen::PenStroke new_stroke;
    auto it = new_pts.begin();
    new_stroke.set_data(new_pts.size(), [&it](auto &&x, auto &&y) {
        x = it->x();
        y = it->y();
        it++;
    });
    return new_stroke;
}

TEST(test_data, create_mini_couch) {
    // set maybe_label to null before creating mini_couch
    using namespace az;
    using namespace az::data;
    CouchDatasetReader reader;
    reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
            "Couch_Digit_195",
            "Couch_GB1_188",
            "Couch_GB2_195",
            "Couch_Letter_195",
            "Couch_Symbol_130"
    });
    std::ofstream idx_out(get_tmp_root("MiniCouch/font.idx"), std::ios::out | std::ios::binary);
    std::ofstream dat_out(get_tmp_root("MiniCouch/font.dat"), std::ios::out | std::ios::binary);

    size_t sample_count = 1;
    auto sample_num = static_cast<int32_t>(reader.get_keys().size() * sample_count);
    ostream_write_buffer(idx_out, &sample_num, 4);

    int32_t dat_offset = 0;
    auto write_dat_fwd_ptr = [&dat_offset, &dat_out](auto *var, std::streamsize length) {
        ostream_write_buffer(dat_out, var, length);
        dat_offset += static_cast<int32_t>(length);
    };

    // ignore these fields
    const int32_t osw_index = 0;
    const int32_t idx_index = 0;
    const unsigned char sample_state = '\0';
    const uint16_t get_time_point_num = 0;

    for (size_t i = 0; i < sample_count; i++)
    for (auto &key: reader.get_keys()) {
        SPDLOG_INFO("key={}", key);
        const auto character = reader.select(key);
        auto data = *character; // deep-copy a sample
        float width = std::numeric_limits<uint16_t>::max() - 2;
        data.fit_into_keep_ratio(width, width);
        data.move_tl_to({0, 0});
        std::string gbk_label;
        try {
            gbk_label = UTF8ToGBK(key);
        } catch (std::exception &e) {
            SPDLOG_ERROR("failed to convert {}. Exception: {}", key, e.what());
            throw e;
        }
        ASSERT_EQ(key, GBKToUTF8(gbk_label));
        std::vector<char> word_code;
        word_code.reserve(gbk_label.size());
        for (auto c: gbk_label) { word_code.push_back(c); }

        auto word_length = static_cast<unsigned char>(word_code.size());
        auto point_num = static_cast<uint16_t>(data.as_points().size());
        auto line_num = static_cast<uint16_t>(data.data.size());

        ostream_write_buffer(idx_out, &sample_state, 1);
        ostream_write_buffer(idx_out, &osw_index, 4);
        ostream_write_buffer(idx_out, &idx_index, 4);
        ostream_write_buffer(idx_out, &dat_offset, 4);
        write_dat_fwd_ptr(&word_length, 1);
        write_dat_fwd_ptr(word_code.data(), 1 * word_length);
        write_dat_fwd_ptr(&point_num, 2);
        write_dat_fwd_ptr(&line_num, 2);
        write_dat_fwd_ptr(&get_time_point_num, 2);
        for (auto &stroke: data.data) {
//            auto new_stroke = shrink_stroke(stroke);
            auto& new_stroke = stroke;
            auto pts_num = static_cast<uint16_t>(new_stroke.count());
            write_dat_fwd_ptr(&pts_num, 2);
            for (const auto &p: new_stroke.as_points()) {
                auto ix = static_cast<uint16_t>(std::round(p.x()));
                auto iy = static_cast<uint16_t>(std::round(p.y()));
                write_dat_fwd_ptr(&ix, 2);
                write_dat_fwd_ptr(&iy, 2);
            }
        }
    }
}

TEST(test_data, read_mini_couch) {
    using namespace az;
    using namespace az::data;
    CouchDatasetReader reader;
    reader.sync_load_all(get_tmp_root(""), {
            "MiniCouch",
    });
    for (auto &key: reader.get_keys()) {
        SPDLOG_INFO("key={}", key);
    }
}

// FIXME: figure out why we lose ~2000 kinds of chars here
TEST(test_data, debug_mini_couch) {
    using namespace az;
    using namespace az::data;
    CouchDatasetReader mini_reader;
    mini_reader.sync_load_all(get_tmp_root(""), {
            "MiniCouch",
    });

    CouchDatasetReader reader;
    reader.sync_load_all(get_dataset_root("SCUT_IRAC/Couch"), {
            "Couch_Digit_195",
            "Couch_GB1_188",
            "Couch_GB2_195",
            "Couch_Letter_195",
            "Couch_Symbol_130"
    });

    auto keys = reader.get_keys();
    auto mini_keys = mini_reader.get_keys();
    std::vector<UCharType> diff_set;
    std::set_difference(keys.begin(), keys.end(), mini_keys.begin(), mini_keys.end(), std::back_inserter(diff_set));
    for (auto &key: diff_set) {
        SPDLOG_INFO(key);
    }
}
