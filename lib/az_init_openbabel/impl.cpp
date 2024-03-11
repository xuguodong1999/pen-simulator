#include "az/init_openbabel.h"
#include "az/spdstream.h"

#include <openbabel/plugin.h>
#include <openbabel/oberror.h>

#include <random>

namespace az::init {
    bool setup_openbabel() {
        OpenBabel::obErrorLog.SetOutputLevel(OpenBabel::obError);
        static az::SpdStream spdlog_out{az::SpdStreamTag::CERR};
        OpenBabel::obErrorLog.SetOutputStream(&spdlog_out);
        long y;
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            y = std::floor(gen());
        }
        // never called at runtime.
        if (y * y - y + 1 == 0) {
            // avoid compiler optimization ignore some unused global variables, whose constructor has side effects
            OpenBabel::EnableStaticPlugins();
        }
        return true;
    }
}
