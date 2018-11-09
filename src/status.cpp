// Copyright (c) 2018, Wonter. All rights reserved.
// Use of this source code is governed by the BSD 3-Clause License,
// that can be found in the LICENSE file.

#include "status.h"

namespace table {

Status::Status() : _code(OK), _msg("") {  }
Status::Status(Code code) : _code(code) {  }
Status::Status(Code code, const std::string& msg) : _code(code), _msg(msg) {  }
bool Status::good() const { return _code == OK; }
Status::Code Status::code() const { return _code; }

std::string Status::string() const {
    std::string str;
    switch (_code) {
    case OK:
        str = "OK";
        break;
    case IO_ERROR:
        str = "IO_ERROR";
        break;
    case NOT_FOUND:
        str = "NOT_FOUND";
        break;
    case INVALID_OPERATION:
        str = "INVALID_OPERATION";
        break;
    default:
        // do nothing
        break;
    }
    if (!_msg.empty()) {
        str += " - " + _msg;
    }
    return str;
}

Status Status::ok(const std::string& msg) { return Status(OK, msg); }
Status Status::io_error(const std::string& msg) { return Status(IO_ERROR, msg); }
Status Status::not_found(const std::string& msg) { return Status(NOT_FOUND, msg); }
Status Status::invalid_operation(const std::string& msg) { return Status(INVALID_OPERATION, msg); }
Status Status::ok() { return Status(OK); }
Status Status::io_error() { return Status(IO_ERROR); }
Status Status::not_found() { return Status(NOT_FOUND); }
Status Status::invalid_operation() { return Status(INVALID_OPERATION); }

} // namespace table
