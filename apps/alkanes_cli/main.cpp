#include "az/core/init_spdlog.h"

#include "az/math/alkane_isomer.h"

#include <boost/program_options.hpp>
#include <boost/numeric/conversion/cast.hpp>

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
alkanes-cli is a cli tool for dumping n-carbon isomers into disk.
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
    const auto n = vm.at("n").as<int64_t>();
    const auto output_path = vm.at("output").as<std::string>();
    const int64_t n_limit = sizeof(az::math::SmiHashType) * 8 / 2;
    if (n > n_limit || n < 1) {
        SPDLOG_ERROR("num of carbon {} exceed limit of [{}, {}]", n, 1, n_limit);
        return EXIT_FAILURE;
    }
//    az::math::AlkaneIsomerUtil::get_isomers_sync(boost::numeric_cast<int8_t>(n));
    az::math::AlkaneIsomerUtil::dump_isomers_sync(boost::numeric_cast<int8_t>(n), output_path);
    return EXIT_SUCCESS;
}

void add_help_info(po::options_description &desc) {
    desc.add_options()
            ("help,h", "print usage of az-data-cli")
            ("n,n", po::value<int64_t>()->default_value(10),
             "indicate the N for n-carbon isomers")
            ("output,o", po::value<std::string>()->default_value("./"),
             "path to a directory, \nwhere we store alkane isomers");
}
