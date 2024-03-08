#pragma once

#include <boost/rational.hpp>
#include <cstdlib>
#include <cmath>

namespace az {
    using FracType = boost::rational<int64_t>;

    template<typename Out, typename T>
    requires std::is_integral_v<Out>
    Out frac_to_int(const boost::rational <T> &f) {
        return static_cast<Out>(std::round(static_cast<double>(f.numerator()) / f.denominator()));
    }

    template<typename Out, typename T>
    requires std::is_floating_point_v<Out>
    Out frac_to_fp(const boost::rational <T> &f) {
        return static_cast<Out>(f.numerator()) / f.denominator();
    }
}