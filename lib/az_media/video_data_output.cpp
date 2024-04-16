#include "az/media/skia_utils.h"

#include "az/media/data_output.h"
#include "az/pen/pen_context.h"

#include <SkPaint.h>
#include <SkCanvas.h>
#include <SkBitmap.h>

#include <boost/process.hpp>

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

    boost::asio::io_context io;
    bp::async_pipe iso(io), ise(io);

    struct {
        std::string cmd;
        std::vector<std::string> args;
    } task = {"ffmpeg", {
            "-y",
            "-f", "rawvideo",
//            "-hwaccel", "cuda",
//            "-hwaccel_output_format", "cuda",
            "-vcodec", "rawvideo",
            "-framerate", "60",
            "-s", fmt::format("{}x{}", param.frame_width, param.frame_height).data(),
            "-pix_fmt", "rgba",
            "-i", "-",
            "-c:v", "nvenc_hevc",
//            "-c:v", "h264_nvenc",
//            "-c:v", "libx265",
//            "-c:v", "h264",
            "-r", "60",
            "-movflags", "frag_keyframe+empty_moov",
            "-f", "mp4",
            "-loglevel", "error",
            "-",
    }};

    bp::opstream cp_in;
    bp::child cp(
            group, bp::search_path(task.cmd), bp::args(task.args),
            bp::std_in = cp_in,
            bp::std_out > iso,
            bp::std_err > ise
    );

    std::function<void()> stream_pump;
    stream_pump = [&, buf = std::vector<uint8_t>(1024)]() mutable {
        boost::asio::async_read(iso, boost::asio::buffer(buf), [&](error_code ec, size_t const nread) {
            bool const eof = (ec == boost::asio::error::eof);
            SPDLOG_INFO("nread: {} (eof:{})", nread, eof);
            if (ec && !eof) {
                SPDLOG_ERROR("async_read: {} ({})", ec.message(), nread);
                return;
            }
            std::copy(buf.begin(), buf.begin() + nread, std::back_inserter(buffer));
            if (eof) { return; }
            stream_pump();
        });
    };
    stream_pump();
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
        io.run();
        return buffer;
    }
}
