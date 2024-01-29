#ifndef ARGUMENTPARSER_HPP
#define ARGUMENTPARSER_HPP

#include <string>
#include <vector>
#include <span>
#include <optional>
#include <string_view>
#include <map>
#include <iostream>
#include <memory>
#include <variant>
#include <functional>

using UnitArgument = std::function<void()>;
using VariableArgument = std::function<bool(const char*)>;

using Argument = std::variant<
    UnitArgument,
    VariableArgument
>;

class Option {
public:
    std::optional<char> shortName;
    std::optional<std::string> longName;
    std::string description;
    Argument argument;

    Option(
        std::optional<char> shortName,
        std::optional<std::string> longName,
        Argument argument
    );
};

class ArgumentParser {
private:
    const int argc;
    char** argv;
    std::vector<Option> options;
    std::optional<VariableArgument> defaultArg{};
    std::ostream& errorStream;

    int index;
    bool doneHandling;

    const char* programName;

    const char* getNextCstr();
    bool parseValue(const char* opt);
    bool parseShort(std::string_view opt);
    bool parseLong(std::string_view opt);

    bool handleOption(
        const Option* option,
        const std::string& optionName,
        bool supportsArg = true
    );

    const Option* getOption(char name);
    const Option* getOption(std::string_view name);
public:
    ArgumentParser(int argc, char** argv, std::ostream& errorStream);
    ArgumentParser& addOpt(Option argument);
    ArgumentParser& addOpt(
        std::optional<char> shortName,
        std::optional<std::string> longName,
        Argument argument
    );
    ArgumentParser& setDefaultArg(VariableArgument arg);
    bool parse();
    void printHelp(std::ostream& stream);
    void printVersion(std::ostream& stream, std::string_view version);
};

template<typename T>
std::function<void()> argumentAssign(T* assignTo, T valueToAssign) {
    return [assignTo, valueToAssign](){
        *assignTo = valueToAssign;
    };
}

template<typename T>
std::function<bool(const char*)> argumentString(T* assignTo) {
    return [assignTo](const char* value){
        *assignTo = T{value};
        return true;
    };
}

template<typename T>
std::function<bool(const char*)> argumentAppendString(
    std::vector<T>* appendTo
) {
    return [appendTo](const char* value) {
        appendTo->push_back(T{value});
        return true;
    };
}

std::function<bool(const char*)> argumentInt(int* assignTo);

#endif

