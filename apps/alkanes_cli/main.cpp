#include "az/core/init_spdlog.h"

#include "az/math/alkane_isomer.h"

#include <boost/program_options.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <string>
#include <vector>
#include <limits>
#include <unordered_set>

namespace po = boost::program_options;

using namespace az;
using namespace az::math;

static void add_help_info(po::options_description &desc);

int main(int argc, char **argv) {
    std::srand(0022); // NOLINT
    /// 1. parse arguments
    az::core::setup_spdlog();
    po::options_description desc(R"(
---------------------------------------------------------------------------
az-data-cli is a visualization tool for svg and handwriting data synthesis.
az-data-cli supports trace of svg paths and handwriting points.
---------------------------------------------------------------------------
Usage)", 120);
    add_help_info(desc);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        SPDLOG_INFO(desc);
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

void add_help_info(po::options_description &desc) {
    desc.add_options()
            ("help,h", "print usage of az-data-cli")
            ("couch", po::value<std::string>()->default_value("Couch"),
             R"(path to SCUT_IRAC/Couch dataset.
see www.hcii-lab.net/data/scutcouch/cn/download.html
----------------------------------------------------
SCUT_IRAC
└── Couch (point to this directory)
    ├── Couch_Digit_195
    ├── Couch_GB1_188
    ├── Couch_GB2_195
    ├── Couch_Letter_195
    └── Couch_Symbol_130
)")
            ("makemeahanzi", po::value<std::string>()->default_value("graphics.txt"),
             R"(path to makemeahanzi graphics.txt.
see https://github.com/skishore/makemeahanzi
--------------------------------------------
makemeahanzi
└── graphics.txt (point to this file)
)")
            ("input", po::value<std::string>()->default_value("example.json"),
             "path to a json file containing inputs,  \nsee apps/az_data_cli/example.json for usage")
            ("output", po::value<std::string>()->default_value(""),
             "path to output frames directory, \ndisplay with opencv highgui if left empty")
            ("threads", po::value<size_t>()->default_value(8),
             "when output is specified, run in parallel")
            ("limit", po::value<size_t>()->default_value(std::numeric_limits<size_t>::max()),
             "if samples in json exceed limit, break generation")
            ("multiply", po::value<int>()->default_value(0),
             "if > 0, ignore input and display number multiply examples")
            ("isomer", po::value<int>()->default_value(0),
             "ignore input and display alkane isomer examples");
}
