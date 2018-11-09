// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.
//
// Here we implemented a random number generator.
// We use Linear Congruential Generator algorithm, and it's simple and useful.
// r[i] = (a * r[i - 1] + c) % m

#ifndef TABLE_RANDOM_H
#define TABLE_RANDOM_H

#include "common.h"

namespace table {

class Random {
TABLE_PUBLIC:
    explicit Random(uint32_t seed) : _seed(seed) {  }
    ~Random() = default;

    uint32_t rand() {
        // Source: RtlUniform from Native API
        //
        // Modulus     m =  2^31 - 1
        // Increment   c =  2147483587
        // Multiplier  a =  2147483629
        static const uint64_t M = 2147483647;
        static const uint64_t A = 2147483629;
        static const uint64_t C = 2147483587;

        _seed = (static_cast<uint64_t>(_seed) * A + C) % M;

        return _seed;
    }

TABLE_PRIVATE:
    uint32_t  _seed;
};

} // namespace table

#endif