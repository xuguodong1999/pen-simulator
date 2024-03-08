#include "az/data/couch_item.h"

using namespace az;
using namespace az::data;

void CouchItem::read(std::istream &in, IndexType stroke_num) {
    auto &strokes = data.data;
    strokes.reserve(stroke_num);
    strokes.resize(stroke_num);
    for (auto &stroke: strokes) {
        uint16_t pts_num;
        in.read((char *) &pts_num, 2);
        stroke.set_data(pts_num, [&in](auto &x, auto &y) {
            uint16_t ix, iy;
            in.read((char *) &ix, 2);
            in.read((char *) &iy, 2);
            x = ix;
            y = iy;
        });
    }
}