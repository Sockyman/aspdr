#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "Error.hpp"
#include <vector>
#include <iostream>

class ErrorHandler {
public:
    virtual std::vector<Error>& getErrors() = 0;
    virtual const std::vector<Error>& getErrors() const = 0;

    void error(const Error& err);

    bool hasErrors() const;
    bool hasErrorLevel(Error::Level level) const;

    void displayErrors(std::ostream& stream);
};


#endif

