#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "Error.hpp"
#include "Result.hpp"
#include <vector>
#include <iostream>

class ErrorHandler {
public:
    virtual std::vector<Error>& getErrors() = 0;
    virtual const std::vector<Error>& getErrors() const = 0;

    template<typename T>
    Result<T> error(const Error& err);

    VoidResult voidError(const Error& err);

    bool hasErrors() const;
    bool hasErrorLevel(Error::Level level) const;

    void displayErrors(std::ostream& stream);
};

template<typename T> 
Result<T> ErrorHandler::error(const Error& err) {
    this->getErrors().push_back(err);
    return Result<T>::getErrorResult();
}

#endif

