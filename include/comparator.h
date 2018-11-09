// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.
//
// We allow you to define your own comparator.
// Just inherit Comparator and override compare()

#ifndef TABLE_COMPARATOR_H
#define TABLE_COMPARATOR_H

#include "byte_array.h"

namespace table {

class Comparator {
public:
    // return value:
    // < 0: a < b
    // = 0: a == b
    // > 0: a > b
    virtual int compare(const ByteArray& lhs, const ByteArray& rhs) const = 0;
};

} // namespace table

#endif