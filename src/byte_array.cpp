// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "byte_array.h"

#include "common.h"

namespace table {

bool operator==(const ByteArray& lhs, const ByteArray &rhs) {
    if (lhs.data() == rhs.data()) {
        return true;
    }

    if (lhs.size() != rhs.size()) {
        return false;
    }

    return memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

ByteArray::ByteArray() : _size(0), _data("") {  }
ByteArray::ByteArray(const char* data, size_t size) : _size(size), _data(data) {  }
ByteArray::ByteArray(const char* s) : _size(strlen(s)), _data(s) {  }
ByteArray::ByteArray(const std::string& s) : _size(s.length()), _data(s.data()) {  }
ByteArray::~ByteArray() {  }

bool ByteArray::empty() const {
    return _size == 0;
}

size_t ByteArray::size() const {
    return _size;
}

char* ByteArray::data() {
    return const_cast<char*>(_data);
}

const char* ByteArray::data() const {
    return _data;
}

void ByteArray::assign(const char* data, size_t size) {
    _data = data;
    _size = size;
}

} // namespace table