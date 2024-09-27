#include "az/data/couch_item.h"

using namespace az;
using namespace az::data;

void CouchItem::read(std::istream &in, IndexType stroke_num) {
    auto &strokes = data.data;
    strokes.reserve(stroke_num);
    strokes.resize(stroke_num);
    for (auto &stroke: strokes) {
        uint16_t pts_num;
        istream_read_buffer(in, &pts_num, 2);
        stroke.set_data(pts_num, [&in](auto &x, auto &y) {
            uint16_t ix, iy;
            istream_read_buffer(in, &ix, 2);
            istream_read_buffer(in, &iy, 2);
            x = ix;
            y = iy;
        });
    }
}