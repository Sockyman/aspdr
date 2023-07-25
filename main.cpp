#include "Driver.hpp"
#include "Assembler.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <SpdrFirmware/Mode.hpp>
#include <fstream>
#include <iostream>

int main() {
    Driver driver{};
    int code = driver.parseFile();

    Assembler assembler{driver.statements};
    assembler.assemble();

    return code;
}

