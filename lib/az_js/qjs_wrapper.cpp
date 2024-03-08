#include "az/js/qjs_wrapper.h"

#include <quickjspp.hpp>
#include <spdlog/fmt/fmt.h>

#include <mutex>
#include <thread>

using namespace az;
using namespace az::js;

namespace az::js::impl::mathjax_src {
    extern const char *CODE_BUFFER;
}

std::string QjsWrapper::mathjax_get_vars_json(std::string_view name) {
    auto js_code = fmt::format(R"xxxxx(
JSON.stringify(globalThis.MathJax.{})
)xxxxx", name);
    try {
        return ctx->eval(js_code).as<std::string>();
    } catch (qjs::exception &e) {
        throw std::runtime_error(e.get().as<std::string>());
    }
}

std::string QjsWrapper::mathjax_tex_to_svg(std::string_view str) {
    try {
        return fn_tex_to_svg(str);
    } catch (qjs::exception &e) {
        throw std::runtime_error(e.get().as<std::string>());
    }
}

std::string QjsWrapper::mathjax_tex_to_json(std::string_view str) {
    try {
        return fn_tex_to_json(str);
    } catch (qjs::exception &e) {
        throw std::runtime_error(e.get().as<std::string>());
    }
}

namespace {
    // https://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const/8147213#8147213
    struct QjsWrapperImpl : QjsWrapper {
        QjsWrapperImpl() = default;
    };
}

std::shared_ptr<QjsWrapper> QjsWrapper::get_instance(const bool &allow_destroy, const bool &force_destroy) {
    /**
     * update: https://docs.mathjax.org/en/latest/web/typeset.html said mathjax keep previous
     *      content until manually clear.
     *      I found this behavior when rendering the same label element twice.
     * TODO: figure out how to clear mathjax history in liteAdaptor
     * 1. mathjax 3.2.2 has serious memory growth when processing im2latex-100k,
     *      node-js official demo also leaks over 2 GB,
     *      so, we just destroy the whole js engine to bring memories back
     * 2. thread_local is best choice but mingw has a bug on it, no idea
     * 3. quickjs is not thread safe from c perspective
     * 4. multithreading is a necessity
     */
    static const uint8_t max_count = 88;
//#if 0
#ifndef __MINGW64__
    thread_local std::shared_ptr<QjsWrapper> qjs = nullptr;
    thread_local uint8_t counter = 0;
    if (!qjs || force_destroy || (allow_destroy && (++counter % max_count == 0))) {
        qjs = std::make_shared<QjsWrapperImpl>();
        counter = 0;
    }
    return qjs;
#else
    static std::mutex mutex;
    static std::unordered_map<std::thread::id, std::shared_ptr<QjsWrapper>> qjs_factory;
    static std::unordered_map<std::thread::id, int64_t> qjs_counter;
    std::shared_ptr<QjsWrapper> qjs = nullptr;
    auto id = std::this_thread::get_id();
    {
        std::unique_lock lk(mutex);
        if (qjs_factory.size() > std::thread::hardware_concurrency()) {
            qjs_factory.clear();
            qjs_counter.clear();
        }
        auto it = qjs_factory.find(id);
        if (it != qjs_factory.end()) {
            qjs = it->second;
            if (allow_destroy) {
                if (force_destroy) {
                    it = qjs_factory.end();
                } else {
                    auto count = qjs_counter[id]++;
                    if (count % max_count == 0) {
                        it = qjs_factory.end();
                    }
                }
            }
        }
        if (it != qjs_factory.end()) {
            return qjs;
        }
    }
    // TODO: check if this way is really thread safe
    qjs = std::make_shared<QjsWrapperImpl>();
    {
        std::unique_lock lk(mutex);
        qjs_factory[id] = qjs;
        qjs_counter[id] = 0;
    }
    return qjs;
#endif
}

QjsWrapper::QjsWrapper() {
    runtime = std::make_shared<qjs::Runtime>();
    ctx = std::make_shared<qjs::Context>(*runtime);
    ctx->eval(az::js::impl::mathjax_src::CODE_BUFFER, "@xgd/mathjax-fork", JS_EVAL_TYPE_MODULE);
    ctx->eval(R"(
import * as MathJax from '@xgd/mathjax-fork';
globalThis.MathJax = MathJax;
)", "<eval>", JS_EVAL_TYPE_MODULE);
    fn_tex_to_svg = ctx->eval(R"(
globalThis.MathJax.Wrapper.texToSvg;
)").as<decltype(fn_tex_to_svg)>();
    fn_tex_to_json = ctx->eval(R"(
globalThis.MathJax.Wrapper.texToJSON;
)").as<decltype(fn_tex_to_json)>();
    fn_unicode_char_to_hex = ctx->eval(R"(
globalThis.MathJax.unicodeCharToHex;
)").as<decltype(fn_unicode_char_to_hex)>();
    fn_count_unicode = ctx->eval(R"(
globalThis.MathJax.countUnicode;
)").as<decltype(fn_count_unicode)>();
}

UBufType QjsWrapper::convert_unicode_str_to_buffer(std::string_view sv) {
    auto qjs = QjsWrapper::get_instance(false);
    try {
        return qjs->fn_unicode_char_to_hex(sv);
    } catch (qjs::exception &e) {
        throw std::runtime_error(e.get().as<std::string>());
    }
}

int64_t QjsWrapper::count_unicode(std::string_view sv) {
    auto qjs = QjsWrapper::get_instance(false);
    return qjs->fn_count_unicode(sv);
}

void QjsWrapper::clear() {
}