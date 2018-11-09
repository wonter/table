// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.
//
// You can put a string or object into ByteArray
// And all operations are shallow copies
// We use it to copy faster since we need to copy key and value in SkipList, and std::string can't meet our need

#ifndef TABLE_BYTE_ARRAY_H
#define TABLE_BYTE_ARRAY_H

#include <string>

namespace table {

class ByteArray {
public:
    ByteArray();
    ~ByteArray();
    ByteArray(const char* data, size_t size);
    ByteArray(const char* s);
    ByteArray(const std::string& s);

    bool empty() const;
    size_t size() const;
    char* data();
    const char* data() const;
    void assign(const char* data, size_t size);

private:
    size_t      _size;
    const char *_data;
};

bool operator==(const ByteArray& lhs, const ByteArray &rhs);

} // namespace table

#endif