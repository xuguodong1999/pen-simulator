#include "az/spdstream.h"

#include <spdlog/spdlog.h>

#include <string_view>
#include <iostream>

using namespace az;

SpdStreamBuf::SpdStreamBuf(std::ostringstream &osm, const SpdStreamTag &tag) : osm(osm), tag(tag) {
}

SpdStreamBuf::~SpdStreamBuf() {
#ifdef __clang__
    std::string buf = this->str();
    std::string_view view = buf;
#else
    std::string_view view = this->view();
#endif
    // FIXME: do not use spdlog in destructor of any global/static variable
    //  https://github.com/gabime/spdlog/issues/2113
    if (!view.empty()) {
        std::cerr << view << std::endl;
    }
    flush();
}

int SpdStreamBuf::sync() {
#ifdef __clang__
    std::string buf = this->str();
    std::string_view view = buf;
#else
    std::string_view view = this->view();
#endif
    while (!view.empty() && (view.back() == '\n' || view.back() == '\r')) {
        view = view.substr(0, view.size() - 1);
    }
    while (!view.empty() && (view.front() == '\n' || view.front() == '\r')) {
        view = view.substr(1, view.size() - 1);
    }
    if (!view.empty()) {
        switch (tag) {
            case SpdStreamTag::CERR:
                SPDLOG_ERROR(view);
                break;
            case SpdStreamTag::CLOG:
                SPDLOG_WARN(view);
                break;
            case SpdStreamTag::COUT:
            default:
                SPDLOG_INFO(view);
                break;
        }
    }
    flush();
    return 0;
}

void SpdStreamBuf::flush() {
    str("");
    osm.flush();
}

SpdStream::SpdStream(const SpdStreamTag &tag) : buf(osm, tag), std::ostream(&buf) {
}
