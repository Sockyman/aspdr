#ifndef ERROR_HPP
#define ERROR_HPP

#include "Location.hpp"
#include <exception>
#include <optional>
#include <iostream>
#include <string>
#include <sstream>

#define ASSEMBLER_ERROR(MESSAGE) do { \
        std::stringstream ss{}; \
        ss << __FILE__ << ":" \
            << __LINE__ << ": " \
            << __PRETTY_FUNCTION__ << ": " \
            << (MESSAGE); \
        throw AssemblerError{ss.str()}; \
    } while (false)

#define ASSEMBLER_ASSERT(ASSERTION, MESSAGE) do { \
        if (!(ASSERTION)) { \
            ASSEMBLER_ERROR((MESSAGE)); \
        } \
    } while (false)

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

void printError(
    std::ostream& stream,
    const std::string& message,
    const std::optional<Location>& location = {});

#endif

