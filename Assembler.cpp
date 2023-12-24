#include "Assembler.hpp"
#include "Error.hpp"
#include "Context.hpp"
#include "Driver.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <functional>
#include <filesystem>
#include <format>
#include <span>

Assembler::Assembler()
:   symbols{},
    parsedFiles{},
    binaryFiles{},
    sections{},
    instructionSet{}
{
    this->createSection("code", true, 0x0000);
    this->createSection("var", false, 0x8000);
}

Assembler::~Assembler() {
    //int i = 0;
    for (auto& parsed : this->parsedFiles) {
        //std::cout << parsed.first << '\n';
        delete parsed.second;
    }
}

void hexdump(std::span<char> bytes) {
    const unsigned long bytesPerLine = 16;
    for (std::size_t i = 0; i < bytes.size(); i += bytesPerLine) {
        std::cout << std::format("{:04x}: ", i);

        for (
            std::size_t j = 0;
            j < std::min(bytes.size() - i, bytesPerLine);
            ++j
        ) {
            if (j == bytesPerLine / 2) {
                std::cout << ' ';
            }

            std::cout << std::format(
                "{:02x} ",
                static_cast<std::uint8_t>(bytes[i + j])
            );
        }
        std::cout << '\n';
    }
}

/// Assembling

Context Assembler::passes(const std::string& fileName) {
    const int maxPasses = 2;
    std::vector<Error> previousErrors{};

    int pass = 0;
    for (;;) {
        Context context = Context{this};

        this->assemble(context, fileName, {});

        if (!context.hasErrors() 
            || previousErrors == context.getErrors()
            || context.hasErrorLevel(Error::Level::Syntax)
            || pass++ == maxPasses
        ) {
            return context;
        }

        previousErrors = std::move(context.errors);
    }
}

bool Assembler::run(const std::string& fileName) {
    Context context = passes(fileName);

    if (context.hasErrors()) {
        context.displayErrors(std::clog);
        return false;
    }

    for (const auto& symbol : this->symbols) {
        std::cerr
            << symbol.first
            << std::format(" = {:#04x} ; {}\n", symbol.second, symbol.second);
    }

    auto& bytes = context.sections["code"].bytes;

    std::cout.write(bytes.data(), bytes.size());
    //hexdump(bytes);

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

    //std::cout << fileName << ": " << std::filesystem::exists(fileName) << '\n';

    auto file = openFile(context, fileName, location);

    if (file.isErr()) {
        return file.into<ParsedFile*>();
    }

    auto parsed = this->parseFile(file.getOk(), fileName);
    if (!parsed) {
        return context.error<ParsedFile*>({
            Error::Level::Syntax,
            location,
            "failed to parse file"
        });
    }

    std::fclose(file.getOk());

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

    //std::cout << "id: " << identifier.getOk() << '\n';

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

bool fileExists(const std::filesystem::path& fileName) {
    //std::cout << fileName << "\n";
    if (!std::filesystem::exists(fileName)
        || std::filesystem::is_directory(fileName)
    ) {
        return false;
    }
    return true;
}

Result<std::string> getFileName(
    Context& context,
    const std::string& fileName,
    const std::optional<Location>& location
) {
    if (fileExists(fileName)) {
        return {fileName};
    } 

    auto stdPath = std::filesystem::path{
        "/home/sockyman/src/spdr-software/asm-new/"
    } / fileName;

    if (fileExists(stdPath)) {
        return stdPath.string();
    }

    std::stringstream ss{};
    ss << "no such file '" << fileName << "'";
    return context.error<std::string>({
        Error::Level::Fatal, location, ss.str()
    });
}

Result<FILE*> openFile(
    Context& context,
    const std::string& fileName,
    const std::optional<Location>& location
) {
    if (fileName == "stdin") {
        return stdin;
    }

    auto resolvedPath = getFileName(context, fileName, location);
    if (resolvedPath.isErr()) {
        return resolvedPath.into<FILE*>();
    }

    FILE* file = std::fopen(resolvedPath.getOk().c_str(), "r");
    ASSEMBLER_ASSERT(file, "failed to open file");
    return file;
}

//VoidResult Assembler::defineMacro(Macro macro, std::vector<Statement*> statements);

//Result<const MicroSequence*> Assembler::findInstruction(const Instruction& lookup) {
//}
