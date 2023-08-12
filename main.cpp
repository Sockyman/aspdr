#include "Driver.hpp"
#include "Assembler.hpp"
#include "Result.hpp"
#include "Error.hpp"
#include "parser.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <optional>

int main(int argc, char** argv) {
    Assembler assembler{};

    if (argc > 1) {
        FILE* file = std::fopen(argv[1], "r");
        assembler.run(file, argv[1]);
        std::fclose(file);
    } else {
        assembler.run(stdin, "stdin");
    }

    //assembler.printSymbols(std::clog);

    return 0;
}

