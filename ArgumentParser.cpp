#include "ArgumentParser.hpp"
#include <cstring>
#include <stdexcept>
#include <format>

Option::Option(
    std::optional<char> shortName,
    std::optional<std::string> longName,
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
    options{},
    errorStream{errorStream},
    index{0},
    doneHandling{false} {}

const char* ArgumentParser::getNextCstr() {
    if (this->index >= this->argc) {
        return nullptr;
    }
    return this->argv[this->index++];
}

ArgumentParser& ArgumentParser::addOpt(Option argument) {
    this->options.push_back(std::move(argument));
    return *this;
}

ArgumentParser& ArgumentParser::addOpt(
    std::optional<char> shortName,
    std::optional<std::string> longName,
    Argument argument
) {
    return this->addOpt({shortName, longName, argument});
}

ArgumentParser& ArgumentParser::setDefaultArg(VariableArgument arg) {
    this->defaultArg = arg;
    return *this;
}

bool ArgumentParser::parseValue(const char* opt) {
    if (std::strlen(opt) >= 1 && opt[0] == '-' && !this->doneHandling) {
        return this->parseShort(&opt[1]);
    }
    
    if (this->defaultArg) {
        (*this->defaultArg)(opt);
    }
    return true;
}

bool ArgumentParser::handleOption(
    const Option* option,
    const std::string& optionName,
    bool supportsArg
) {
    if (!option) {
        this->errorStream 
            << std::format("{}: unrecognized option '{}'\n",
                this->programName,
                optionName
            );

        // TODO: Error handling
        return false;
    }

    if (std::holds_alternative<VariableArgument>(option->argument)) {
        auto next = this->getNextCstr();
        if (!next 
            || !supportsArg
            || (std::strlen(next) > 0 && next[0] == '-')
        ) {
            this->errorStream 
                << std::format("{}: missing argument to option '{}'\n",
                    this->programName,
                    optionName
                );

            // TODO: Error handling
            return false;
        }

        if (!std::get<VariableArgument>(option->argument)(next)) {
            this->errorStream << std::format(
                "{}: failed to parse command line argument "
                "'{}' to option '{}'\n",
                this->programName,
                *next,
                optionName
            );
            return false;
        }
    } else {
        std::get<UnitArgument>(option->argument)();
    }

    return true;
}

bool ArgumentParser::parseShort(std::string_view opt) {
    if (opt.size() >= 1 && opt[0] == '-') {
        return this->parseLong(opt.substr(1));
    }

    for (std::size_t i = 0; i < opt.length(); ++i) {
        char c = opt[i];
        if (!this->handleOption(
            this->getOption(c),
            std::format("-{}", c),
            i == opt.length() - 1
        )) {
            return false;
        }
    }
    return true;
}

bool ArgumentParser::parseLong(std::string_view opt) {
    if (opt.length() == 0) {
        this->doneHandling = true;
        return true;
    }
    return this->handleOption(this->getOption(opt), std::format("--{}", opt), true);
}

const Option* ArgumentParser::getOption(char name) {
    for (auto& arg : this->options) {
        if (arg.shortName == name) {
            return &arg;
        }
    }
    return nullptr;
}

const Option* ArgumentParser::getOption(std::string_view name) {
    for (auto& arg : this->options) {
        if (arg.longName == name) {
            return &arg;
        }
    }
    return nullptr;
}

bool ArgumentParser::parse() {
    // --long
    // --longwithval value
    // --longwithval=value
    // -ssssss
    // -sssssS value
    // -sssssS=value
    // --
    // value (anything else)

    bool success = true;
    this->programName = this->getNextCstr();
    while (index < argc) {
        if (!this->parseValue(this->getNextCstr())) {
            success = false;
        }
    }
    return success;
}

void ArgumentParser::printHelp(std::ostream& stream) {
    stream << "Usage:\n " << this->programName << " <options...>";
    if (this->defaultArg.has_value()) {
        stream << " <arg>";
    }
    stream << "\n\nOptions:\n";

    for (const auto& option : this->options) {
        stream << ' ';
        if (option.shortName) {
            stream << "-" << *option.shortName;
        } else {
            stream << "  ";
        }

        stream << "  ";

        if (option.longName) {
            stream << "--" << *option.longName << " ";
        }

        if (std::holds_alternative<VariableArgument>(option.argument)) {
            stream << "  <arg>";
        }
        stream << '\n';
    }
}

void ArgumentParser::printVersion(std::ostream& stream, std::string_view version) {
    stream << this->programName << " " << version << '\n';
}

std::function<bool(const char*)> argumentInt(int* assignTo) {
    return [assignTo](const char* value) {
        try {
            *assignTo = std::stol(value, nullptr, 0);
            return true;
        } catch (std::invalid_argument&) {}
            catch (std::out_of_range&) {}
        return false;
    };
}


