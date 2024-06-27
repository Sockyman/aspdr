#include "Assembler.hpp"
#include "Error.hpp"
#include "Context.hpp"
#include "Driver.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <format>
#include <span>


Assembler::Assembler(
    SectionMode sectionMode,
    const std::span<std::string_view> includePath,
    std::optional<std::string> prelude
)
:   symbols{},
    parsedFiles{},
    includePath{includePath},
    sectionMode{sectionMode},
    prelude{prelude},
    binaryFiles{},
    sections{},
    instructionSet{}
{
    switch (sectionMode) {
        case SectionMode::ROM:
            createSection("code", true, 0x0000, 0x8000);
            createSection("var", false, 0x8000, 0xff00);
            break;
        case SectionMode::RAM:
            createSection("code", true, 0x9000, 0xc000);
            createSection("var", false, 0xc000, 0xff00);
            break;
    }

    symbols[Identifier{"ROM"}] = sectionMode == SectionMode::ROM ? 1 : 0;
    symbols[Identifier{"RAM"}] = sectionMode == SectionMode::RAM ? 1 : 0;
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
        //std::clog << "PASS: " << pass << '\n';
        Context context = Context{this};

        if (this->prelude.has_value()) {
            this->assemble(context, this->prelude.value());
        }

        this->assemble(context, fileName);

        if (!context.hasErrors() 
            || previousErrors == context.getErrors()
            || context.hasErrorLevel(Error::Level::Syntax)
        ) {
            return context;
        }

        if (pass++ >= maxPasses) {
            context.error(
                Error::Level::Fatal,
                std::format(
                    "maximum assembler passes ({}) exceeded.",
                    maxPasses
                )
            );
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

    auto bytes = context.sections["code"].getBytes();

    std::cout.write(bytes.data(), bytes.size());
    //hexdump(bytes);

    return true;
}

bool Assembler::assemble(
    Context& context,
    const std::string& fileName,
    std::optional<Location> location
) {
    auto parsed = this->getParsedFile(context, fileName, location);
    if (!parsed) {
        return false;
    }

    if (parsed->once && context.markAsIncluded(fileName)) {
        return true;
    }

    return this->assemble(context, parsed->statements);
}


bool Assembler::assemble(
    Context& context,
    const std::vector<Statement*>& statements
) {
    for (auto& statement : statements) {
        statement->assemble(context);
    }
    return true;
}


/// Parsing

Block* Assembler::getParsedFile(
    Context& context,
    const std::string& fileName,
    std::optional<Location> location
) {
    if (this->parsedFiles.contains(fileName)) {
        return this->parsedFiles[fileName];
    }

    //std::cout << fileName << ": " << std::filesystem::exists(fileName) << '\n';

    auto file = openFile(context, fileName, this->includePath, location);

    if (!file.has_value()) {
        return {};
    }

    auto parsed = this->parseFile(file.value(), fileName);
    if (!parsed) {
        context.error(
            Error::Level::Syntax,
            "failed to parse file",
            location
        );
        return nullptr;
    }

    std::fclose(*file);

    this->parsedFiles[fileName] = parsed;
    return this->parsedFiles[fileName];
}

Block* Assembler::parseFile(
    FILE* file,
    const std::string& fileName
) {
    Driver driver{fileName};
    yyin = file;

    if (driver.parseFile()) {
        return nullptr;
    }
    return driver.parsed;
}

/// Symbols

std::optional<std::int64_t> Assembler::resolveSymbol(
    const std::optional<Identifier>& identifier
) const {
    if (!identifier) {
        return std::nullopt;
    }

    if (!this->symbols.contains(*identifier)) {
        return std::nullopt;
    }

    return this->symbols.at(*identifier);
}

bool Assembler::assignSymbol(
    Context& context,
    const Location& location,
    const std::optional<Identifier>& identifier,
    std::int64_t value
) {
    if (!identifier) {
        return false;
    }

    auto existing = this->resolveSymbol(*identifier);
    if (existing && *existing != value) {
        std::stringstream ss{};
        ss << "redefinition of \'" << *identifier << "\'";
        context.error(Error::Level::Fatal, ss.str(), location);
        return false;
    }

    this->symbols[identifier.value()] = value;
    return true;
}

void Assembler::printSymbols(std::ostream& stream) {
    for (auto& symbol : this->symbols) {
        stream
            << '#'
            << symbol.first
            << std::format(" = {:#04x} ; {}\n", symbol.second, symbol.second);
    }
}

/// Sections

void Assembler::createSection(
    std::string name,
    bool writable,
    std::int64_t start,
    std::int64_t end
) {
    sections[name] = SectionInfo{name, writable, start, end};
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

std::optional<std::string> getFileName(
    Context& context,
    const std::string& fileName,
    const std::span<std::string_view> includePath,
    const std::optional<Location>& location
) {
    //std::filesystem::path filePath{fileName};

    if (fileExists(fileName)) {
        return {fileName};
    } 

    for (auto prefix : includePath) {
        auto stdPath = std::filesystem::path{prefix} / fileName;

        if (fileExists(stdPath)) {
            return stdPath.string();
        }
    }

    context.error(
        Error::Level::Fatal,
        std::format("no such file '{}'", fileName),
        location
    );
    return std::nullopt;
}

std::optional<FILE*> openFile(
    Context& context,
    const std::string& fileName,
    const std::span<std::string_view> includePath,
    const std::optional<Location>& location
) {
    if (fileName == "stdin") {
        return stdin;
    }

    auto resolvedPath = getFileName(context, fileName, includePath, location);
    if (!resolvedPath.has_value()) {
        return {};
    }

    FILE* file = std::fopen(resolvedPath.value().c_str(), "r");
    ASSEMBLER_ASSERT(file, "failed to open file");
    return file;
}

//bool Assembler::defineMacro(Macro macro, std::vector<Statement*> statements);

//std::optional<const MicroSequence*> Assembler::findInstruction(const Instruction& lookup) {
//}
