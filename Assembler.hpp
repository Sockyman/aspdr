#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Statement.hpp"
#include "Identifier.hpp"
#include "Section.hpp"
#include "SectionInfo.hpp"
#include <SpdrFirmware/InstructionSet.hpp>
#include <SpdrFirmware/MicroSequence.hpp>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <optional>
#include <span>
#include <string_view>

enum class SectionMode {
    ROM,
    RAM,
};

class Context;

class Assembler {
private:
    std::map<Identifier, std::int64_t> symbols;
    std::map<std::string, Block*> parsedFiles;
    const std::span<std::string_view> includePath;
    const SectionMode sectionMode;
    const std::optional<std::string> prelude;

    //std::map<int, std::map<int, std::int64_t>> numericLabels;

    Block* getParsedFile(
        Context& context,
        const std::string& fileName,
        std::optional<Location> location = {}
    );

    Block* parseFile(
        FILE* file,
        const std::string& fileName
    );

    Context passes(const std::string& fileName);

public:
    std::map<std::string, std::vector<char>> binaryFiles;

    std::map<std::string, SectionInfo> sections;
    const InstructionSet instructionSet;

    Assembler(SectionMode sectionMode, const std::span<std::string_view> includePath, std::optional<std::string> prelude);
    ~Assembler();

    Assembler(const Assembler&) = delete;
    Assembler& operator=(const Assembler&) = delete;


    bool run(const std::string& fileName);

    bool assemble(
        Context& context,
        const std::string& fileName, 
        std::optional<Location> location = {}
    );

    bool assemble(
        Context& context,
        const std::vector<Statement*>& statements
    );

    std::optional<std::int64_t> resolveSymbol(
        const std::optional<Identifier>& identifier
    ) const;

    bool assignSymbol(
        Context& context,
        const Location& location,
        const std::optional<Identifier>& identifier,
        std::int64_t value
    );

    void createSection(std::string name, bool writable, std::int64_t start, std::int64_t);

    void printSymbols(std::ostream& stream);

    //bool defineMacro(Macro macro, std::vector<Statement*> statements, int uid);
};

std::optional<std::string> getFileName(
    Context& context,
    const std::string& filename,
    const std::span<std::string_view> includePath,
    const std::optional<Location>& location = {}
);

std::optional<FILE*> openFile(
    Context& context,
    const std::string& fileName,
    const std::span<std::string_view> includePath,
    const std::optional<Location>& location = {}
);

#endif

