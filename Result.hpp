#ifndef RESULT_HPP
#define RESULT_HPP

#include "Error.hpp"
#include <variant>
#include <functional>

template<typename T>
class Result {
private:
    std::variant<ErrorList, T> value;

    template<typename V, typename U>
    Result<V> collect(const Result<U>& result);
public:
    using OkType = T;

    Result(std::variant<ErrorList, T> value);
    Result(T okValue);
    Result(Error errValue);
    Result();

    static Result<T> fromErrorList(ErrorList errors);

    bool isOk() const;
    bool isErr() const;
    const T& getOk() const;
    const ErrorList& getErr() const;

    template<typename U>
    Result<U> then(const Result<U>& result);

    template<typename V, typename U, typename F>
    Result<V> then(const Result<U>& result, const F& fn);

    template<typename U, typename F>
    Result<U> map(const F& fn);

    static Result<T> fromOptional(const std::optional<T>& option, const Error& onNone);

    template<typename U>
    U into() const;

    Result<T> otherwise(const Result<T>& result);


    explicit operator bool() const;
};

using VoidResult = Result<std::monostate>;

/// Implementation

template<typename T>
template<typename V, typename U>
Result<V> Result<T>::collect(const Result<U>& result) {
    ASSEMBLER_ASSERT(result.isErr() || this->isErr(), "at least one result must be err.");

    if (result.isErr() && this->isErr()) {
        return Result<V>{this->getErr().join(result.getErr())};
    }

    if (result.isErr()) {
        return Result<V>{result.getErr()};
    }
    return Result<V>{this->getErr()};
}

template<typename T>
Result<T>::Result(std::variant<ErrorList, T> value) : value{value} {
    //std::cout << "Errorlist\n";
}

template<typename T>
Result<T>::Result(T okValue) : value{okValue} {
    //std::cout << "Ok\n";
}

template<typename T>
Result<T>::Result(Error errValue) : value{errValue} {
    //std::cout << "Error\n";
}

template<typename T>
Result<T>::Result() : value{T{}} {}


template<typename T>
Result<T> Result<T>::fromErrorList(ErrorList errors) {
    VoidResult result{};
    result.value = errors;
    return result.into<Result<T>>();
}

template<typename T>
Result<T> Result<T>::fromOptional(const std::optional<T>& option, const Error& onNone) {
    if (option)
        return Result<T>{*option};
    return Result<T>{onNone};
}

template<typename T>
bool Result<T>::isOk() const {
    return std::holds_alternative<T>(this->value);
}

template<typename T>
bool Result<T>::isErr() const {
    return !this->isOk();
}

template<typename T>
const T& Result<T>::getOk() const {
    ASSEMBLER_ASSERT(this->isOk(), "getOk() called on non ok Result.");
    return std::get<T>(this->value);
}

template<typename T>
const ErrorList& Result<T>::getErr() const {
    ASSEMBLER_ASSERT(this->isErr(), "getErr() called on non err Result.");
    return std::get<ErrorList>(this->value);
}

template<typename T>
template<typename U>
Result<U> Result<T>::then(const Result<U>& result) {
    if (this->isOk() && result.isOk()) {
        return result;
    }
    return this->collect<U, U>(result);
}

template<typename T>
template<typename V, typename U, typename F>
Result<V> Result<T>::then(const Result<U>& result, const F& fn) {
    if (this->isOk() && result.isOk()) {
        return fn(this->getOk(), result.getOk());
    }
    return this->collect<V>(result);
}

template<typename T>
template<typename U, typename F>
Result<U> Result<T>::map(const F& fn) {
    if (this->isOk()) {
        return fn(this->getOk());
    }
    return Result<U>{this->getErr()};
}

template<typename T>
template<typename U>
U Result<T>::into() const {
    ASSEMBLER_ASSERT(this->isErr(), "into() can only convert err.")
    return U{this->getErr()};
}

template<typename T>
Result<T> Result<T>::otherwise(const Result<T>& result) {
    if (this->isOk())
        return *this;
    return result;
}

template<typename T>
Result<T>::operator bool() const {
    return this->isOk();
}

#endif

