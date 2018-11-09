// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.
//
// Options to control the behavior of a table.
// Passe it to Table::Open.

#ifndef TABLE_OPTIONS_H
#define TABLE_OPTIONS_H

#include "comparator.h"

namespace table {

struct Options {
    // Comparator used to define the order of keys in the table.
    // Default: a comparator that uses lexicographic byte-wise ordering
    Comparator* comparator;

    // If true, the table will be created if it is missing.
    // Default: false
    bool create_if_missing;

    // If true, an error is raised if the table already exists.
    // Default: false
    bool error_if_exists;

    // If true, the table will dump all entries when it is closed.
    // Default: true
    bool dump_when_close;

    // Time-To-Live of a read operation.
    // Default: 2000
    int read_ttl_msec;

    // Maximum size of a single file.
    // Default: 1073741824(1GB)
    off_t max_file_size;

    // Create an Options object with default values for all fields.
    Options();
};

} // namespace table

#endif