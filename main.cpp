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

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }

    Driver driver{};
    int code = driver.parseFile();

    Assembler assembler{driver.statements};
    assembler.assemble();

    return code;
}

