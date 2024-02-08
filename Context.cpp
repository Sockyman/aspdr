#include "Context.hpp"

Context::Context(Assembler* assembler)
:   assembler{assembler},
    sections{},
    currentSection{"code"},
    includedFiles{},
    fileNames{},
    frames{},
    scope{}

{
    for (auto& sec : assembler->sections) {
        sections[sec.first] = Section{&sec.second};
    }
}

Section& Context::getSection() {
    return sections[currentSection];
}

bool Context::changeSection(
    const Location& location,
    const std::string& newSection
) {
    if (!this->sections.contains(newSection)) {
        std::stringstream ss{};
        ss << "section \'" << newSection << "\' does not exist";
        return this->error(Error::Level::Fatal, ss.str(), location);
    }
    this->currentSection = newSection;
    return true;
}

std::vector<Error>& Context::getErrors() {
    return this->errors;
}

const std::vector<Error>& Context::getErrors() const {
    return this->errors;
}

bool Context::markAsIncluded(const std::string& fileName) {
    if (this->includedFiles.contains(fileName)) {
        return true;
    }
    this->includedFiles.insert(fileName);
    return false;
}

std::optional<Identifier> Context::qualify(
    const Location& location,
    const UnqualifiedIdentifier& unqualified
) {
    return unqualified.qualify(*this, location, this->scope);
}

void Context::setScope(const Identifier& id) {
    this->scope = id;
}

