#include "Error.hpp"

AssemblerError::AssemblerError(std::string message) : message{message} {
}

const char* AssemblerError::what() const noexcept {
    return this->message.c_str();
}

Error::Error(Location location, std::string message) : location{location}, message{message} {}

Error::Error(std::string message) : location{std::nullopt}, message{message} {}

const char* Error::what() const noexcept {
    return this->message.c_str();
}

void Error::display() {
    std::cerr << *this << '\n';
}

std::ostream& operator<<(std::ostream& stream, Error& error) {
    if (error.location) {
        stream << *error.location << ": ";
    }
    stream << "error: " << error.message;
    return stream;
}

FatalError::FatalError(Location location, std::string message) : Error{location, message} {}
FatalError::FatalError(std::string message) : Error{message} {}

PassError::PassError(Location location, std::string message) : Error{location, message} {}
PassError::PassError(std::string message) : Error{message} {}


