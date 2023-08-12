#include "ErrorHandler.hpp"
#include <algorithm>

VoidResult ErrorHandler::voidError(const Error& err) {
    return this->error<std::monostate>(err);
}

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

