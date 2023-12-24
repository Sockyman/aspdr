#include "Driver.hpp"
#include "Assembler.hpp"
#include "Result.hpp"
#include "Error.hpp"
#include "parser.hpp"
#include "Options.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <optional>
#include <format>
#include <functional>

struct Foo {
    std::function<bool(int)> f;
};

int main(int argc, char** argv) {
    std::string outfile;
    std::string infile;
    bool help = false;
    bool version = false;

    ArgumentParser argumentParser{argc, argv, std::clog};
    argumentParser
        .addOptValue<StringArgument>('o', "outfile", &outfile)
        .addOptAssign('h', "help", &help, true)
        .addOptAssign('v', "version", &version, true)
        .setDefaultArgValue<StringArgument>(&infile)
        .parse();

    std::cout << std::format("'{}' -> '{}' h: {} v: {}\n", infile, outfile, help, version);
    return 0;

    Assembler assembler{};

    std::string file = argc > 1 ? argv[1] : "stdin";
    bool success = assembler.run(file);

    //assembler.printSymbols(std::clog);

    return success ? 0 : 1;
}

