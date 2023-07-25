#include "Identifier.hpp"
#include <stdexcept>
#include <algorithm>

Identifier::Identifier() {}; 

Identifier::Identifier(std::vector<std::string> value) : value{value} {}

Identifier::Identifier(std::string str) : Identifier{std::vector{str}} {
}

void Identifier::push(std::string str) {
    this->value.push_back(str);
}

std::strong_ordering operator<=>(const Identifier& id0, const Identifier& id1) {
    int len = std::min(id0.value.size(), id1.value.size());
    for (int i = 0; i < len; ++i) {
        if (id0.value[i] != id1.value[i])
            return id0.value[i] <=> id1.value[i];
    }
    return id0.value.size() <=> id1.value.size();
}

std::ostream& operator<<(std::ostream& stream, const Identifier& id) {
    if (id.value.size() == 0)
        return stream;

    stream << id.value[0];

    for (std::size_t i = 1; i < id.value.size(); ++i) {
        stream << '.' << id.value[i];
    }
    return stream;
}

UnqualifiedIdentifier::UnqualifiedIdentifier() {}

UnqualifiedIdentifier::UnqualifiedIdentifier(std::size_t depth, Identifier identifier)
: identifier{identifier}, depth{depth} {}

Identifier UnqualifiedIdentifier::qualify(const Identifier& id) {
    if (id.value.size() < this->depth)
        throw std::logic_error{__PRETTY_FUNCTION__}; // TODO: Error handling.

    std::vector<std::string> v{};
    v.insert(v.end(), id.value.begin(), id.value.begin() + this->depth);
    v.insert(v.end(), this->identifier.value.begin(), this->identifier.value.end());
    return {v};
}

