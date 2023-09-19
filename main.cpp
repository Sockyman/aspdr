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

#include "Frame.hpp"

int main(int argc, char** argv) {
    Assembler assembler{};

    std::string file = argc > 1 ? argv[1] : "stdin";
    bool success = assembler.run(file);

    //assembler.printSymbols(std::clog);

    return success ? 0 : 1;
}

