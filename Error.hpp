#ifndef ERROR_HPP
#define ERROR_HPP

#include "Location.hpp"
#include <exception>
#include <optional>
#include <iostream>
#include <string>
#include <sstream>

#define ASSEMBLER_ERROR(MESSAGE) \
    { \
        std::stringstream ss{}; \
        ss << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << ": " << (MESSAGE); \
        throw AssemblerError{ss.str()}; \
    }

class AssemblerError : public std::exception {
public:
    AssemblerError(std::string message);
    virtual const char* what() const noexcept override;
    std::string message;
};

class Error : public std::exception {
private:
public:
    std::optional<Location> location;
    std::string message;

    Error(Location location, std::string message);
    Error(std::string message);

    virtual const char* what() const noexcept override;
    void display();
};

std::ostream& operator<<(std::ostream& stream, Error& error);

class FatalError : public Error {
public:
    FatalError(Location location, std::string message);
    FatalError(std::string message);
};

class PassError : public Error {
public:
    PassError(Location location, std::string message);
    PassError(std::string message);
};

#endif

