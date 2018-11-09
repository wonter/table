// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.
//
// Thread safety of Table
// ------------------------
// Writers require external synchronization
// Readers require time of a read operation less than Options::read_ttl_msec

#ifndef TABLE_TABLE_H
#define TABLE_TABLE_H

#include "status.h"
#include "options.h"
#include "byte_array.h"

namespace table {

class Table {
public:
    Table(const Options& options, const std::string& filename);

    ~Table();

    // Open the table with the specified "name" that in constructor.
    // You should call Close() when it is no longer needed.
    // Returns OK on success.
    Status open();

    // Close the table.
    // You should not do any operation after the table is closed.
    // Returns OK on success.
    Status close();

    // Persist the entries to disk.
    // Returns OK on success.
    Status dump();

    // Store the corresponding value in *value if the table contains an entry for "key".
    // If value == nullptr, the corresponding value is not set.
    // Returns OK on success.
    Status get(const ByteArray& key, std::string* value);

    // Set the table entry for "key" to "value".
    // Returns OK on success.
    Status put(const ByteArray& key, const ByteArray& value);

    // Remove the table entry (if any) for "key".
    // It is an error if "key" did not exist in the table.
    // Returns OK on success.
    Status del(const ByteArray& key);

    // Non-copying
    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;

private:
    class TableImpl;
    TableImpl *_impl;
};

} // namespace table

#endif