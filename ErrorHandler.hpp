#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "Error.hpp"
#include <vector>
#include <iostream>
#include <optional>

class Failure {
public:
    template<typename T>
    operator std::optional<T>() const;
    template<typename T>
    operator T*() const;
    operator bool() const;

    void discard() const;
};

template<typename T>
Failure::operator std::optional<T>() const {
    return std::nullopt;
}

template<typename T>
Failure::operator T*() const {
    return nullptr;
}


class ErrorHandler {
public:
    virtual std::vector<Error>& getErrors() = 0;
    virtual const std::vector<Error>& getErrors() const = 0;

    [[nodiscard]]
    Failure error(const Error& err);

    [[nodiscard]]
    Failure error(Error::Level level, std::string message, std::optional<Location> location = {});

    bool hasErrors() const;
    bool hasErrorLevel(Error::Level level) const;

    void displayErrors(std::ostream& stream);
};


#endif

