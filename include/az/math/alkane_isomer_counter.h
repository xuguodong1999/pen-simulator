#pragma once
// see https://github.com/snljty/CountAlkylIsomer
#include "az/math/config.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>
#include <vector>

namespace az::math {
    using BIntType = boost::multiprecision::cpp_int;

    class AZMATH_EXPORT PolyaIsomerCounter {
        std::vector<BIntType> A, P, Q, C;
        inline static const BIntType CONSTANT_2 = 2;
        inline static const BIntType CONSTANT_3 = 3;
        inline static const BIntType CONSTANT_6 = 6;
        inline static const BIntType CONSTANT_8 = 8;
        inline static const BIntType CONSTANT_24 = 24;

        void a_i(const size_t &m);

        void p_i(const size_t &m);

        void q_i(const size_t &m);

        void c_i(const size_t &m);

        void mk_space(const size_t &buffer_size);

    public:
        std::pair<BIntType, BIntType> count_alkane_radical(size_t num_of_c);
    };
}