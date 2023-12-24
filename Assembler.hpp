#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "Statement.hpp"
#include "Identifier.hpp"
#include "Section.hpp"
#include "SectionInfo.hpp"
#include "Driver.hpp"
#include "ErrorHandler.hpp"
#include "Macro.hpp"
#include <SpdrFirmware/InstructionSet.hpp>
#include <SpdrFirmware/MicroSequence.hpp>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <optional>
#include <tuple>
#include <filesystem>

class Context;

class Assembler {
private:
    std::map<Identifier, std::int64_t> symbols;
    std::map<std::string, ParsedFile*> parsedFiles;

    Result<ParsedFile*> getParsedFile(
        Context& context,
        const std::string& fileName,
        std::optional<Location> location = {}
    );

    std::optional<ParsedFile*> parseFile(
        FILE* file,
        const std::string& fileName
    );

    Context passes(const std::string& fileName);

public:
    std::map<std::string, std::vector<char>> binaryFiles;

    std::map<std::string, SectionInfo> sections;
    const InstructionSet instructionSet;

    std::map<Macro, std::pair<std::vector<Statement*>, int>> macros;

    Assembler();
    ~Assembler();

    bool run(const std::string& fileName);

    VoidResult assemble(
        Context& context,
        const std::string& fileName, 
        std::optional<Location> location = {}
    );

    VoidResult assemble(
        Context& context,
        const std::vector<Statement*>& statements
    );

    std::optional<std::int64_t> resolveSymbol(
        const Result<Identifier>& identifier
    ) const;

    VoidResult assignSymbol(
        Context& context,
        const Location& location,
        const Result<Identifier>& identifier,
        std::int64_t value
    );

    void createSection(std::string name, bool writable, std::int64_t start);

    void printSymbols(std::ostream& stream);


    //VoidResult defineMacro(Macro macro, std::vector<Statement*> statements, int uid);
};

Result<std::string> getFileName(
    Context& context,
    const std::string& filename,
    const std::optional<Location>& location = {}
);

Result<FILE*> openFile(
    Context& context,
    const std::string& fileName,
    const std::optional<Location>& location = {}
);

#endif

