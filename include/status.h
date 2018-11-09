// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.
//
// A Status encapsulates the result of an operation.
// It may indicate success, or it may indicate an error with an associated error message.

#ifndef TABLE_STATUS_H
#define TABLE_STATUS_H

#include <string>

namespace table {

class Status {
public:
    enum Code {
        OK = 0,
        IO_ERROR = 1,
        NOT_FOUND = 2,
        INVALID_OPERATION = 3,
    };

    Status();
    explicit Status(Code code);
    Status(Code code, const std::string& msg);
    ~Status() = default;

    bool good() const;
    Code code() const;
    std::string string() const;

    static Status ok(const std::string& msg);
    static Status io_error(const std::string& msg);
    static Status not_found(const std::string& msg);
    static Status invalid_operation(const std::string& msg);
    static Status ok();
    static Status io_error();
    static Status not_found();
    static Status invalid_operation();

private:

    Code         _code;
    std::string  _msg;
};

} // namespace table

#endif
