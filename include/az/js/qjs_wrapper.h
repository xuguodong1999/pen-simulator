#pragma once

#include "az/js/config.h"
#include "az/core/encoding.h"

#include <memory>
#include <string>
#include <string_view>
#include <functional>

namespace qjs {
    class Runtime;

    class Context;
}

namespace az::js {
    class AZJS_EXPORT QjsWrapper {
        std::shared_ptr<qjs::Runtime> runtime;
        std::shared_ptr<qjs::Context> ctx;
        std::function<UBufType(std::string_view)> fn_unicode_char_to_hex;
        std::function<int64_t(std::string_view)> fn_count_unicode;
        std::function<std::string(std::string_view)>
                fn_tex_to_svg,
                fn_tex_to_json;
    protected:
        QjsWrapper();

    public:
        QjsWrapper(const QjsWrapper &) = delete;

        QjsWrapper(QjsWrapper &&) = delete;

        QjsWrapper &operator=(const QjsWrapper &) = delete;

        QjsWrapper &operator=(QjsWrapper &&) = delete;

        std::string mathjax_get_vars_json(std::string_view name);

        std::string mathjax_tex_to_svg(std::string_view str);

        std::string mathjax_tex_to_json(std::string_view str);

        static std::shared_ptr<QjsWrapper> get_instance(
                const bool &allow_destroy = true, const bool &force_destroy = false);

        static UBufType convert_unicode_str_to_buffer(std::string_view sv);

        static int64_t count_unicode(std::string_view sv);

        static void clear();
    };
}
