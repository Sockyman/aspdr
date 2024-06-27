#include "Assembler.hpp"
#include "ArgumentParser.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <iostream>
#include <optional>
#include <cstdlib>

// extern const char timestamp[];

int main(int argc, char** argv) {
    enum class Action {
        assemble,
        help,
        version,
    };

    Action action = Action::assemble;

    std::string outfile{};
    const char* infile = "stdin"; 
    bool printSymbols = false;
    std::vector<std::string_view> includePath{};
    SectionMode sectionMode = SectionMode::ROM;
    const char* prelude = std::getenv("ASPDR_PRELUDE");

    const char* env_include = std::getenv("ASPDR_INCLUDE");
    if (env_include) {
        includePath.push_back(env_include);
    }

    ArgumentParser argumentParser{argc, argv, std::clog};
    if (!argumentParser
        .addOpt('o', "outfile", argumentString(&outfile))
        .addOpt('s', "symbols", argumentAssign(&printSymbols, true))
        .addOpt('i', "include", argumentAppendString(&includePath))
        .addOpt('p', "prelude", argumentString(&prelude))
        .addOpt('r', "ram", argumentAssign(&sectionMode, SectionMode::RAM))
        .addOpt({}, "rom", argumentAssign(&sectionMode, SectionMode::ROM))
        .addOpt('h', "help", argumentAssign(&action, Action::help))
        .addOpt('v', "version", argumentAssign(&action, Action::version))
        .setDefaultArg(argumentString(&infile))
        .parse()
    ) {
        return 2;
    }

    bool success = true;

    switch (action) {
        case Action::assemble:
        {
            Assembler assembler{sectionMode, includePath, prelude};

            success = assembler.run(infile);
            if (printSymbols) {
                assembler.printSymbols(std::clog);
            }
        }
            break;
        case Action::help:
            argumentParser.printHelp(std::clog);
            break;
        case Action::version:
            argumentParser.printVersion(std::clog, "");
            break;
    }

    return success ? 0 : 1;
}

