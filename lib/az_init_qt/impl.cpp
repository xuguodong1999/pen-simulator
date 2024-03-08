#include "az/init_qt.h"

#include <QtCore/qlogging.h>
#include <QString>
#include <QDebug>

#include <spdlog/spdlog.h>

template<>
struct fmt::formatter<QString> {
    static constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const QString &input, FormatContext &ctx) -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{:s}", input.toLocal8Bit().data());
    }
};

#define QT_FORMAT_STRING "[Qt] [{}] {}"

namespace az::init {
    bool setup_qt() {
        qputenv("QT_IMAGEIO_MAXALLOC", "0");
        qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
        qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
        qputenv("QV4_JIT_CALL_THRESHOLD", "0"); // force jit
        // qputenv("QV4_FORCE_INTERPRETER", "1"); // disable jit
        // qputenv("QV4_MM_AGGRESSIVE_GC", "1");
        qputenv("QML_DISABLE_DISK_CACHE", "1");
        qputenv("QT_DISABLE_SHADER_DISK_CACHE", "1");
        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
            static const QString QML_COMPILER_WARNING = "qt.qml.compiler";
            switch (type) {
                case QtWarningMsg:
                    if (QML_COMPILER_WARNING == context.category) {
                        // ignore too many qv4 engine warning
                        break;
                    }
                    SPDLOG_WARN(QT_FORMAT_STRING, context.category, msg);
                    break;
                case QtCriticalMsg:
                    spdlog::critical(QT_FORMAT_STRING, context.category, msg);
                    break;
                case QtFatalMsg:
                    SPDLOG_ERROR(QT_FORMAT_STRING, context.category, msg);
                    break;
                case QtDebugMsg:
                case QtInfoMsg:
                default:
                    SPDLOG_INFO(QT_FORMAT_STRING, context.category, msg);
                    break;
            }
        });
        return true;
    }
}
