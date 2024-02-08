#include "ErrorHandler.hpp"
#include <algorithm>

bool ErrorHandler::hasErrors() const {
    return this->getErrors().size() > 0;
}

bool ErrorHandler::hasErrorLevel(Error::Level level) const {
    for (auto& err : this->getErrors()) {
        if (err.level >= level) {
            return true;
        }
    }
    return false;
}

void ErrorHandler::displayErrors(std::ostream& stream) {
    for (const auto& err : this->getErrors()) {
        std::clog << err;
    }
}

Failure ErrorHandler::error(const Error& err) {
    this->getErrors().push_back(err);
    return Failure{};
}

Failure ErrorHandler::error(
    Error::Level level,
    std::string message,
    std::optional<Location> location
) {
    return this->error({level, location, message});
}

Failure::operator bool() const {
    return false;
}

void Failure::discard() const {}

