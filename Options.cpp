#include "Options.hpp"
#include <cstring>
#include <stdexcept>

StringArgument::StringArgument(std::string* s) : ValueArgument{s} {}

std::optional<std::string> StringArgument::parse(const char* value) {
    return {value};
}

IntArgument::IntArgument(int* i) : ValueArgument{i} {}

std::optional<int> IntArgument::parse(const char* value) {
    try {
        int i = std::stoi(value);
        return {i};
    } catch (std::invalid_argument) {
    } catch (std::out_of_range) {}

    return {};
}

Option::Option(
    char shortName,
    std::string longName,
    Argument argument
)
:   shortName{shortName},
    longName{longName},
    argument{std::move(argument)} {}


ArgumentParser::ArgumentParser(
    int argc,
    char** argv,
    std::ostream& errorStream
)
:   argc{argc},
    argv{argv},
    arguments{},
    errorStream{errorStream},
    index{0},
    options{} {}

std::optional<const char*> ArgumentParser::getNextCstr() {
    if (this->index >= this->argc) {
        return {};
    }
    return this->argv[this->index++];
}

ArgumentParser& ArgumentParser::addOpt(Option argument) {
    this->arguments.push_back(std::move(argument));
    return *this;
}

ArgumentParser& ArgumentParser::setDefaultArg(std::unique_ptr<VariableArgument> arg) {
    this->defaultArg = std::move(arg);
    return *this;
}

void ArgumentParser::parseValue(std::string_view opt) {
    if (opt.size() >= 1 && opt[0] == '-') {
        this->parseShort(opt.substr(1));
        return;
    }
    
    if (this->defaultArg) {
        std::string s = std::string{opt};
        (*this->defaultArg)->apply(s.c_str());
    }
}

void ArgumentParser::parseShort(std::string_view opt) {
    if (opt.size() >= 1 && opt[0] == '-') {
        this->parseLong(opt.substr(1));
        return;
    }

    this->errorStream << "not implemented!!!\n";
}

void ArgumentParser::parseLong(std::string_view opt) {
    auto option = this->getOption(opt);
    if (!option) {
        this->errorStream << this->programName << ": unrecognized option '" << opt << "'\n";
        // TODO: Error handling
        return;
    }

    if (std::holds_alternative<std::unique_ptr<VariableArgument>>((*option)->argument)) {
        auto next = this->getNextCstr();
        if (!next || (std::strlen(*next) > 0 && (*next)[0] == '-')) {
            this->errorStream << this->programName << ": missing argument to option '" << opt << "'\n";
            // TODO: Error handling
            return;
        }

        std::get<std::unique_ptr<VariableArgument>>((*option)->argument)->apply(*next);
    } else {
        std::get<std::unique_ptr<UnitArgument>>((*option)->argument)->apply();
    }
}

std::optional<const Option*> ArgumentParser::getOption(char name) {
    for (auto& arg : this->arguments) {
        if (arg.shortName == name) {
            return &arg;
        }
    }
    return {};
}

std::optional<const Option*> ArgumentParser::getOption(std::string_view name) {
    for (auto& arg : this->arguments) {
        if (arg.longName == name) {
            return &arg;
        }
    }
    return {};
}

void ArgumentParser::parse() {
    // --long
    // --longwithval value
    // --longwithval=value
    // -ssssss
    // -sssssS value
    // -sssssS=value
    // --
    // value (anything else)

    this->programName = *this->getNextCstr();
    while (index < argc) {
        this->parseValue(*this->getNextCstr());
    }
}

