#include "az/media/skia_utils.h"

#include "az/media/data_output.h"
#include "az/pen/pen_context.h"

#include <SkPaint.h>
#include <SkCanvas.h>
#include <SkBitmap.h>

#include <boost/process.hpp>
#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

using namespace az;
using namespace az::pen;
using namespace az::media;
using namespace az::media::skia;

std::vector<uint8_t> az::media::pen_op_to_video(
        az::pen::PenOp *data,
        const PenOpPaintParam &param
) {
    SkPaint copy_paint = create_paint(param.stroke_color, param.stroke_width);
    auto copy_bitmap = create_bitmap(param.frame_width, param.frame_height);
    SkCanvas canvas(copy_bitmap);

    std::vector<uint8_t> buffer;

    namespace bp = boost::process;
    using namespace std::chrono_literals;
    using boost::system::error_code;
    bp::group group;

    struct {
        std::string cmd;
        std::vector<std::string> args;
    } task = {"ffmpeg", {
            "-y",
            "-f", "rawvideo",
//            "-hwaccel", "cuda",
//            "-hwaccel_output_format", "cuda",
            "-vcodec", "rawvideo",
            "-framerate", "360",
            "-s", fmt::format("{}x{}", param.frame_width, param.frame_height).data(),
            "-pix_fmt", "rgba",
            "-i", "-",
//            "-threads", "6",
            "-c:v", "hevc_nvenc",
//            "-c:v", "h264_nvenc",
//            "-c:v", "libx265",
//            "-c:v", "h264",
            "-r", "360",
            "-movflags", "frag_keyframe+empty_moov",
            "-f", "mp4",
//            "-f", "matroska", // mkv
            "-loglevel", "error",
            "-",
//            "/tmp/1/my_output_ffmpeg.mp4",
    }};

    bp::opstream cp_in;
    bp::ipstream cp_out;
    bp::child cp(
            group, bp::search_path(task.cmd), bp::args(task.args),
            bp::std_in = cp_in,
            bp::std_out > cp_out
//            bp::std_out > iso,
//            bp::std_err > ise
    );
    render_with_skia(data, param, [&](
            const SkBitmap &bitmap, std::string_view action
    ) {
        skia_resize(canvas, bitmap, param);
        cp_in.write((char *) copy_bitmap.getPixels(), sizeof(uint8_t) * param.frame_width * param.frame_height * 4);
    });
    cp_in.flush();
    cp_in.pipe().close();
    cp.wait();
    if (cp.exit_code() != 0) {
        SPDLOG_ERROR("exit code: {}", cp.exit_code());
        return {};
    } else {
        std::ostringstream out;
        out << cp_out.rdbuf();
#ifdef __clang__
        std::string out_str = out.str();
        std::copy(out_str.begin(), out_str.end(), std::back_inserter(buffer));
#else
        std::copy(out.view().begin(), out.view().end(), std::back_inserter(buffer));
#endif
        return buffer;
    }
}
