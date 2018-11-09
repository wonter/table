// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include <iostream>

#include "table.h"

static inline void assert_fatal(const table::Status& s) {
    if (!s.good()) {
        std::cerr << s.string() << std::endl;
        exit(-1);
    }
}

int main() {
    table::Options options;
    options.create_if_missing = true;
    table::Table table(options, "test_table");

    table::Status s = table.open();
    assert_fatal(s);

    table.close();
    return 0;
}