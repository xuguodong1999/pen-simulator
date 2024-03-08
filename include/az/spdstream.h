#pragma once

#include "AzSpdStream_export.h"

#include <sstream>

namespace az {
    enum AZSPDSTREAM_EXPORT SpdStreamTag {
        COUT, CLOG, CERR
    };

    class AZSPDSTREAM_EXPORT SpdStreamBuf : public std::stringbuf {
        std::ostringstream &osm;
        SpdStreamTag tag;
    public:
        explicit SpdStreamBuf(std::ostringstream &osm, const SpdStreamTag &tag);

        ~SpdStreamBuf() override;

        int sync() override;

        void flush();
    };

    class AZSPDSTREAM_EXPORT SpdStream : public std::ostream {
        std::ostringstream osm;
        SpdStreamBuf buf;
    public:
        explicit SpdStream(const SpdStreamTag &tag);
    };
}
