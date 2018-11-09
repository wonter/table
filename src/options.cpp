// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "options.h"

#include "common.h"

namespace table {

class ByteWiseComparator : public Comparator {
public:
    int compare(const ByteArray& lhs, const ByteArray& rhs) const override {
        size_t size = std::min(lhs.size(), rhs.size());
        int cmp = memcmp(lhs.data(), rhs.data(), size);
        if (cmp != 0) {
            return cmp;
        }
        return static_cast<int>(lhs.size()) - static_cast<int>(rhs.size());
    }
};

static Comparator* default_comparator() {
    static ByteWiseComparator cmp;
    return &cmp;
}

Options::Options() :
    comparator(default_comparator()),
    create_if_missing(false),
    error_if_exists(false),
    dump_when_close(true),
    read_ttl_msec(2000),
    max_file_size(1024 * 1024 * 1024) {
}

} // namespace table
