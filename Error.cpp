#include "Error.hpp"
#include <ranges>

AssemblerError::AssemblerError(std::string message) : message{message} {
}

const char* AssemblerError::what() const noexcept {
    return this->message.c_str();
}

Error::Error(Error::Level level, Location location, std::string message) : level{level}, location{location}, message{message} {}

Error::Error(Error::Level level, std::string message) : level{level}, location{std::nullopt}, message{message} {}

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
    stream << "error: " << error.message;
    return stream;
}

ErrorList::ErrorList() : errors{} {}
ErrorList::ErrorList(Error error) : errors{error} {}
ErrorList::ErrorList(std::vector<Error> errors) : errors{errors} {}

bool ErrorList::hasErrors() const {
    return this->errors.size();
}


bool ErrorList::hasFatalErrors() const {
    for (const Error& e : this->errors) {
        if (e.level == Error::Level::Fatal) {
            return true;
        }
    }
    return false;
}

void ErrorList::display(std::ostream& stream) const {
    for (const Error& e : this->errors) {
        stream << e << '\n';
    }
}

ErrorList ErrorList::join(const ErrorList& list) const {
    ErrorList errorList{std::vector{this->errors}};
    errorList.insert(list);
    return errors;
}

void ErrorList::insert(const ErrorList& list) {
    this->errors.insert(this->errors.begin(), list.errors.begin(), list.errors.end());
}

bool ErrorList::operator==(const ErrorList& list) const {
    if (this->errors.size() != list.errors.size())
        return false;

    for (std::size_t i = 0; i < this->errors.size(); ++i) {
        if (this->errors[i] != list.errors[i]) {
            return false;
        }
    }
    return true;
}

