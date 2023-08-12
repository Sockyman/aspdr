#include "Assembler.hpp"
#include "Error.hpp"
#include "Context.hpp"
#include "Driver.hpp"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <functional>
#include <filesystem>
#include <format>

Assembler::Assembler()
:   symbols{},
    parsedFiles{},
    sections{},
    instructionSet{}
{
    this->createSection("code", true, 0);
    this->createSection("var", false, 0x8000);
}

/// Assembling

bool Assembler::run(FILE* file, const std::string& fileName) {
    const int maxPasses = 2;
    std::vector<Error> previousErrors{};

    Context context{this};

    auto parsed = this->parseFile(file, fileName);
    if (!parsed) {
        return false;
    }

    for (int i = 0; i < maxPasses; ++i) {
        previousErrors = std::move(context.errors);
        context = Context{this};

        this->assemble(context, (*parsed)->statements);

        if (!context.hasErrors() 
            || previousErrors == context.getErrors()
            || context.hasErrorLevel(Error::Level::Syntax)
        ) {
            break;
        }
    }

    delete *parsed;

    if (context.hasErrors()) {
        context.displayErrors(std::clog);
        return false;
    }

    auto& bytes = context.sections["code"].bytes;
    //std::cout.write(bytes.data(), bytes.size());

    for (auto b : bytes) {
        std::cout << std::format("{:02x} ", static_cast<std::uint8_t>(b));
    }
    std::cout << '\n';

    return true;
}

VoidResult Assembler::assemble(
    Context& context,
    const std::string& fileName,
    std::optional<Location> location
) {
    auto parsed = this->getParsedFile(context, fileName, location);
    if (parsed.isErr()) {
        return parsed.intoVoid();
    }

    if ((*parsed)->once && context.markAsIncluded(fileName)) {
        return VoidResult{};
    }

    return this->assemble(context, (*parsed)->statements);
}


VoidResult Assembler::assemble(
    Context& context,
    const std::vector<Statement*>& statements
) {
    for (auto& statement : statements) {
        statement->assemble(context);
    }
    return VoidResult{};
}

/// Parsing

Result<ParsedFile*> Assembler::getParsedFile(
    Context& context,
    const std::string& fileName,
    std::optional<Location> location
) {
    if (this->parsedFiles.contains(fileName)) {
        return this->parsedFiles[fileName];
    }

    if (!std::filesystem::exists(fileName)
        || std::filesystem::is_directory(fileName)
    ) {
        std::stringstream ss{};
        ss << "no such file '" << fileName << "'";
        return context.error<ParsedFile*>({
            Error::Level::Fatal, location, ss.str()
        });
    }

    FILE* file = std::fopen(fileName.c_str(), "r");
    auto parsed = this->parseFile(file, fileName);
    if (!parsed) {
        return context.error<ParsedFile*>({Error::Level::Syntax, location, "failed to parse file"});
    }

    std::fclose(file);

    this->parsedFiles[fileName] = *parsed;
    return this->parsedFiles[fileName];
}

std::optional<ParsedFile*> Assembler::parseFile(
    FILE* file,
    const std::string& fileName
) {
    Driver driver{fileName};
    yyin = file;

    if (driver.parseFile()) {
        return {};
    }
    return driver.parsed;
}

/// Symbols

std::optional<std::int64_t> Assembler::resolveSymbol(
    const Result<Identifier>& identifier
) const {
    if (identifier.isErr()) {
        return {};
    }

    if (!this->symbols.contains(identifier.getOk())) {
        return {};
    }
    return {this->symbols.at(identifier.getOk())};
}

VoidResult Assembler::assignSymbol(
    Context& context,
    const Location& location,
    const Result<Identifier>& identifier,
    std::int64_t value
) {
    if (identifier.isErr()) {
        return identifier.intoVoid();
    }

    auto existing = this->resolveSymbol(identifier.getOk());
    if (existing && *existing != value) {
        std::stringstream ss{};
        ss << "redefinition of \'" << identifier.getOk() << "\'";
        return context.voidError({Error::Level::Fatal, location, ss.str()});
    }

    this->symbols[identifier.getOk()] = value;
    return VoidResult{};
}

void Assembler::printSymbols(std::ostream& stream) {
    for (auto& symbol : this->symbols) {
        stream << symbol.first << std::format(" = 0x{:02x}\n", symbol.second);
    }
}

/// Sections

void Assembler::createSection(
    std::string name,
    bool writable,
    std::int64_t start
) {
    sections[name] = SectionInfo{name, writable, start};
}

