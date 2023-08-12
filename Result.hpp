#ifndef RESULT_HPP
#define RESULT_HPP

#include "Error.hpp"
#include <variant>
#include <optional>

template<typename T>
class Result {
private:
    std::optional<T> value;

    Result(std::optional<T> value);
public:
    using OkType = T;

    static Result<T> getErrorResult();

    Result(T okValue);
    Result();

    bool isOk() const;
    bool isErr() const;
    T& getOk();
    const T& getOk() const;

    template<typename U>
    Result<U> then(Result<U> result);
    
    template<typename U, typename F>
    Result<U> map(const F& fn);

    template<typename U>
    Result<U> into() const;

    Result<std::monostate> intoVoid() const;

    explicit operator bool() const;

    T& operator*();
    const T& operator*() const;

    T* operator->();
    const T* operator->() const;
};

using VoidResult = Result<std::monostate>;

/// Implementation

template<typename T>
Result<T> Result<T>::getErrorResult() {
    Result<T> result = Result{std::optional<T>{}};
    ASSEMBLER_ASSERT(result.isErr(), "result is not an error");
    return result;
}

template<typename T>
Result<T>::Result(std::optional<T> value) : value{value} {}

template<typename T>
Result<T>::Result(T okValue) : value{okValue} {
    //std::cout << "Ok\n";
}

template<typename T>
Result<T>::Result() : value{T{}} {}


template<typename T>
bool Result<T>::isOk() const {
    return this->value.has_value();
}

template<typename T>
bool Result<T>::isErr() const {
    return !this->isOk();
}

template<typename T>
T& Result<T>::getOk() {
    ASSEMBLER_ASSERT(this->isOk(), "getOk() called on non ok Result.");
    return this->value.value();
}

template<typename T>
const T& Result<T>::getOk() const {
    ASSEMBLER_ASSERT(this->isOk(), "getOk() called on non ok Result.");
    return this->value.value();
}

template<typename T>
template<typename U, typename F>
Result<U> Result<T>::map(const F& fn) {
    if (this->isOk()) {
        return fn(this->getOk());
    }
    return *this;
}

template<typename T>
template<typename U>
Result<U> Result<T>::then(Result<U> result) {
    if (this->isErr()) {
        return this->into<U>();
    }
    return result;
}

template<typename T>
template<typename U>
Result<U> Result<T>::into() const {
    ASSEMBLER_ASSERT(this->isErr(), "into() can only convert err.")
    return Result<U>::getErrorResult();
}

template<typename T>
Result<std::monostate> Result<T>::intoVoid() const {
    return this->into<std::monostate>();
}

template<typename T>
Result<T>::operator bool() const {
    return this->isOk();
}


template<typename T>
T& Result<T>::operator*() {
    return this->getOk();
}

template<typename T>
const T& Result<T>::operator*() const {
    return this->getOk();
}

template<typename T>
T* Result<T>::operator->() {
    return &this->getOk();
}

template<typename T>
const T* Result<T>::operator->() const {
    return &this->getOk();
}

#endif

