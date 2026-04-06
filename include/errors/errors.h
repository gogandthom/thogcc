#pragma once

#include <format>
#include <stdexcept>
#include <string>

namespace thogcc::errors {

struct ErrorBase : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct CommandLineError : public ErrorBase {
    CommandLineError(const std::string& msg)
        : ErrorBase(std::format("CommandLineError: {}", msg)) {}
};

struct ParseError : public ErrorBase {
    ParseError(const std::string& msg) : ErrorBase(std::format("ParseError: {}", msg)) {}
};

struct SemaError : public ErrorBase {
    SemaError(const std::string& msg) : ErrorBase(std::format("SemanticAnalysisError: {}", msg)) {}
};

}  // namespace thogcc::errors
