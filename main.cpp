#include "Driver.hpp"
#include "Assembler.hpp"
#include "Result.hpp"
#include "Error.hpp"
#include "parser.hpp"
#include "ArgumentParser.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <optional>
#include <format>
#include <functional>

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

    ArgumentParser argumentParser{argc, argv, std::clog};
    if (!argumentParser
        .addOpt('o', "outfile", argumentString(&outfile))
        .addOpt('s', "symbols", argumentAssign(&printSymbols, true))
        .addOpt('i', "include", argumentAppendString(&includePath))
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
            Assembler assembler{sectionMode, includePath};

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
            argumentParser.printVersion(std::clog, DATETIME);
            break;
    }

    return success ? 0 : 1;
}

