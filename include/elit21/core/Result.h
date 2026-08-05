#pragma once
#include <optional>
#include <string>
#include <utility>

namespace elit21 {
template<class T>
class Result {
public:
    static Result success(T value) { return Result(std::move(value), {}); }
    static Result failure(std::string error) { return Result(std::nullopt, std::move(error)); }
    bool ok() const noexcept { return value_.has_value(); }
    explicit operator bool() const noexcept { return ok(); }
    const T& value() const { return value_.value(); }
    T& value() { return value_.value(); }
    T take() { return std::move(value_.value()); }
    const std::string& error() const noexcept { return error_; }
private:
    Result(std::optional<T> value, std::string error): value_(std::move(value)), error_(std::move(error)) {}
    std::optional<T> value_;
    std::string error_;
};

template<>
class Result<void> {
public:
    static Result success() { return Result(true, {}); }
    static Result failure(std::string error) { return Result(false, std::move(error)); }
    bool ok() const noexcept { return ok_; }
    explicit operator bool() const noexcept { return ok_; }
    const std::string& error() const noexcept { return error_; }
private:
    Result(bool ok, std::string error): ok_(ok), error_(std::move(error)) {}
    bool ok_;
    std::string error_;
};
}
