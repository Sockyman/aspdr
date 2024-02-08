#include "Identifier.hpp"
#include "Context.hpp"
#include <regex>
#include <sstream>
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
        if (id0.value[i] != id1.value[i]) {
            return id0.value[i] <=> id1.value[i];
        }
    }
    return id0.value.size() <=> id1.value.size();
}

std::ostream& operator<<(std::ostream& stream, const Identifier& id) {
    if (id.value.size() == 0) {
        return stream;
    }

    stream << id.value[0];

    for (std::size_t i = 1; i < id.value.size(); ++i) {
        stream << '.' << id.value[i];
    }
    return stream;
}

UnqualifiedIdentifier UnqualifiedIdentifier::fromString(const std::string& str) {
    static std::regex prefixRegex{"^\\.+"};
    static std::regex idRegex{"[a-zA-Z_][a-zA-Z0-9_]*"};

    std::size_t depth = 0;
    std::smatch match;
    if (std::regex_search(str, match, prefixRegex)) {
        depth = match.str().size();
    }

    UnqualifiedIdentifier id{depth, {}};

    std::sregex_iterator strBegin{str.begin(), str.end(), idRegex};
    std::sregex_iterator strEnd{};
    for (auto i = strBegin; i != strEnd; ++i) {
        id.identifier.push(i->str());
    }

    return id;
}

UnqualifiedIdentifier::UnqualifiedIdentifier() {}

UnqualifiedIdentifier::UnqualifiedIdentifier(
    std::size_t depth,
    Identifier identifier
) : identifier{identifier}, depth{depth} {}

std::optional<Identifier> UnqualifiedIdentifier::qualify(
    Context& context,
    const Location& location,
    const Identifier& id
) const {
    if (id.value.size() < this->depth) {
        std::stringstream ss{};
        ss << "cannot qualify '" << *this << "' with '" << id << "'";

        return context.error(
            Error::Level::Fatal,
            ss.str(),
            location
        );
    }

    std::vector<std::string> v{};
    v.insert(
        v.end(),
        id.value.begin(),
        id.value.begin() + this->depth
    );

    auto it = this->identifier.value.begin();
    const std::string& first = this->identifier.value[0];

    if (first == "MACRO") {
        v.push_back(context.frames.getMacroIdent());
        ++it;
    } else if (first == "LOCAL") {
        v.push_back(context.frames.getLocalIdent());
        ++it;
    }

    v.insert(
        v.end(),
        it,
        this->identifier.value.end()
    );

    return Identifier{v};
}

std::ostream& operator<<(
    std::ostream& stream,
    const UnqualifiedIdentifier& id
) {
    for (std::size_t i = 0; i < id.depth; ++i) {
        stream << '.';
    }
    return stream << id.identifier;
}

void UnqualifiedIdentifier::incrementDepth() {
    ++this->depth;
}

