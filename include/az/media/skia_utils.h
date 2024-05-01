#include "az/core/utils.h"
#include "az/core/geometry_def.h"
#include "az/media/config.h"
#include "az/media/data_output.h"

#include <SkPoint3.h>
#include <SkCanvas.h>
#include <SkBitmap.h>
#include <SkFontMgr.h>

namespace az::pen {
    class PenOp;
}

namespace az::media::skia {
    SkBitmap AZMEDIA_EXPORT create_bitmap(const int &w, const int &h);

    SkMatrix AZMEDIA_EXPORT convert_eigen_33_to_sk_matrix(const TransformMatrix2 &mat);

    SkPaint AZMEDIA_EXPORT create_paint(const SkColor &color, const float &stroke_width = 12);

    sk_sp<SkFontMgr> AZMEDIA_EXPORT get_skia_font_manager();

    SkFont AZMEDIA_EXPORT create_font(const UCharType &label = "😅");

    using fn_on_frame_type = std::function<void(const SkBitmap &, std::string_view)>;

    void AZMEDIA_EXPORT render_with_skia(
            az::pen::PenOp *data,
            const PenOpPaintParam &param,
            const fn_on_frame_type &on_frame
    );

    void AZMEDIA_EXPORT skia_resize(SkCanvas &canvas, const SkBitmap &bitmap, const PenOpPaintParam &param);
}
