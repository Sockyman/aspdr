#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>
#include <vector>
#include <span>
#include <optional>
#include <string_view>
#include <map>
#include <iostream>
#include <memory>
#include <variant>

class UnitArgument {
public:
    virtual bool apply() = 0;
};

class VariableArgument {
public:
    virtual bool apply(const char* value) = 0;
};

template<typename T>
class AssignArgument : public UnitArgument {
public:
    T* assignTo;
    T valueToAssign;

    AssignArgument(T* assignTo, T valueToAssign);
    virtual bool apply() override;
};

template<typename T>
AssignArgument<T>::AssignArgument(T* assignTo, T valueToAssign)
:   assignTo{assignTo}, valueToAssign{valueToAssign} {}

template<typename T>
bool AssignArgument<T>::apply() {
    *this->assignTo = this->valueToAssign;
    return true;
}

template<typename T>
class ValueArgument : public VariableArgument {
public:
    using valueType = T;
    T* assignTo;

    ValueArgument(T* assignTo);
    virtual bool apply(const char* value) override;
    virtual std::optional<T> parse(const char* value) = 0;
};

template<typename T>
ValueArgument<T>::ValueArgument(T* assignTo)
:   assignTo{assignTo} {}

template<typename T>
bool ValueArgument<T>::apply(const char* value) {
    std::optional<T> converted = this->parse(value);
    if (!converted) {
        return false;
    }
    *this->assignTo = *converted;
    return true;
}

class StringArgument : public ValueArgument<std::string> {
public:
    StringArgument(std::string* assignTo);
    virtual std::optional<std::string> parse(const char* value) override;
};

class IntArgument : public ValueArgument<int> {
public:
    IntArgument(int* assignTo);
    virtual std::optional<int> parse(const char* value) override;
};


using Argument = std::variant<
    std::unique_ptr<UnitArgument>,
    std::unique_ptr<VariableArgument>
>;

class Option {
public:
    char shortName;
    std::string longName;
    std::string description;
    Argument argument;

    Option(
        char shortName,
        std::string longName,
        Argument argument
    );
};

class ArgumentParser {
private:
    const int argc;
    char** argv;
    std::vector<Option> arguments;
    std::optional<std::unique_ptr<VariableArgument>> defaultArg{};
    std::ostream& errorStream;

    int index;

    std::map<std::string, std::optional<const char*>> options;
    const char* programName;

    std::optional<const char*> getNextCstr();
    void parseValue(std::string_view opt);
    void parseShort(std::string_view opt);
    void parseLong(std::string_view opt);

    std::optional<const Option*> getOption(char name);
    std::optional<const Option*> getOption(std::string_view name);
public:
    ArgumentParser(int argc, char** argv, std::ostream& errorStream);
    ArgumentParser& addOpt(Option argument);
    template<typename T>
    ArgumentParser& addOptAssign(char shortName, std::string longName, T* assignTo, T valueToAssign);
    template<typename T>
    ArgumentParser& addOptValue(char shortName, std::string longName, T::valueType* assignTo);
    ArgumentParser& setDefaultArg(std::unique_ptr<VariableArgument> arg);
    template<typename T>
    ArgumentParser& setDefaultArgValue(T::valueType* assignTo);
    void parse();
};

template<typename T>
ArgumentParser& ArgumentParser::addOptAssign(
        char shortName,
        std::string longName,
        T* assignTo,
        T valueToAssign
) {
    return this->addOpt({
        shortName,
        longName,
        std::make_unique<AssignArgument<T>>(assignTo, valueToAssign)
    });
}

template<typename T>
ArgumentParser& ArgumentParser::addOptValue(
    char shortName,
    std::string longName,
    T::valueType* assignTo
) {
    return this->addOpt({
        shortName,
        longName,
        std::make_unique<T>(assignTo)
    });
}

template<typename T>
ArgumentParser& ArgumentParser::setDefaultArgValue(T::valueType* assignTo) {
    return this->setDefaultArg(std::make_unique<T>(assignTo));
}


#endif

