#ifndef ERROR_HPP
#define ERROR_HPP

#include "Location.hpp"
#include <exception>
#include <optional>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#define ASSEMBLER_ERROR(MESSAGE) { \
        std::stringstream ss{}; \
        ss << __FILE__ << ":" \
            << __LINE__ << ": " \
            << __PRETTY_FUNCTION__ << ": " \
            << (MESSAGE); \
        throw AssemblerError{ss.str()}; \
    }

#define ASSEMBLER_ASSERT(ASSERTION, MESSAGE) { \
        if (!(ASSERTION)) { \
            ASSEMBLER_ERROR((MESSAGE)); \
        } \
    }

#define UNREACHABLE ASSEMBLER_ERROR("unreachable code reached")

class AssemblerError : public std::exception {
public:
    AssemblerError(std::string message);
    virtual const char* what() const noexcept override;
    std::string message;
};

class Error {
private:
public:
    enum class Level {
        Pass,
        Fatal,
        Syntax,
    };

    Error::Level level;
    std::optional<Location> location;
    std::string message;

    Error(Error::Level level, std::optional<Location> location, std::string message);
    Error(Error::Level level, std::string message);

    void display();

    bool operator==(const Error& other) const;
};

std::ostream& operator<<(std::ostream& stream, const Error& error);

#endif

