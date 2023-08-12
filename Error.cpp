#include "Error.hpp"
#include <ranges>

AssemblerError::AssemblerError(std::string message) : message{message} {
}

const char* AssemblerError::what() const noexcept {
    return this->message.c_str();
}

Error::Error(Error::Level level, std::optional<Location> location, std::string message)
: level{level}, location{location}, message{message} {}

Error::Error(Error::Level level, std::string message)
: level{level}, location{std::nullopt}, message{message} {}

void Error::display() {
    std::clog << *this << '\n';
}

bool Error::operator==(const Error& other) const {
    if (this->location && other.location)
        return *this->location == *other.location;
    if (this->location || other.location)
        return false;
    return this->message == other.message && this->level == other.level;
}

std::ostream& operator<<(std::ostream& stream, const Error& error) {
    if (error.location) {
        stream << *error.location << ": ";
    }
    stream << "\033[31merror:\033[0m " << error.message << '\n';
    return stream;
}

