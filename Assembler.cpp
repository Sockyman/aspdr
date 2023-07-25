#include "Assembler.hpp"
#include "Error.hpp"
#include <stdexcept>
#include <iostream>
#include <cstdio>

Section::Section(std::size_t start, bool isWritable)
: bytes{}, start{start}, offset{0}, writable{isWritable}, counting{true} {
}

void Section::writeInteger(std::int64_t value, int number) {
    if (!this->writable)
        throw std::logic_error(__PRETTY_FUNCTION__); // TODO: Error handling

    if (!this->counting) {
        return;
    }

    this->offset += number;
    for (int i = 0; i < number; ++i) {
        this->bytes.push_back((value >> (i * 8)) & 0xff);
    }
}

void Section::reserve(std::size_t number) {
    if (!this->counting)
        return;

    this->offset += number;
    if (this->writable)
    this->bytes.resize(this->bytes.size() + number, 0);
}

void Section::changeAddress(std::int64_t address) {
    if (!this->counting)
        return;

    if (this->getAddress() > address)
        throw std::logic_error(__PRETTY_FUNCTION__); // TODO: Error handling

    this->offset = address - this->start;
}

std::int64_t Section::getAddress() {
    if (!this->counting)
        throw std::logic_error(__PRETTY_FUNCTION__); // TODO: Error handling
    return this->offset + this->start;
}

void Section::writeByte(std::uint8_t value) {
    this->writeInteger(value, 1);
}

void Section::writeWord(std::int64_t value) {
    this->writeInteger(value, 2);
}

Assembler::Assembler(std::vector<Statement*> statements)
: statements{statements}, instructionSet{} {}

void Assembler::assemble() {
    try {
        Context context{this};
        for (auto statement : statements) {
            std::cout << "@ " << context.section.getAddress() << '\n';
            statement->assemble(context);
        }

        for (auto b : context.section.bytes) {
            std::printf("%02x ", b);
        }
        std::cout << '\n';
    } catch (Error& ex) {
        ex.display();
    }
}

bool Assembler::resolveSymbol(const Identifier& identifier, std::int64_t& value) const {
    if (!this->symbols.contains(identifier))
        return false;
    value = this->symbols.at(identifier);
    return true;
}

void Assembler::assignSymbol(const Identifier& identifier, std::int64_t value) {
    std::cout << identifier << ": " << value << '\n';
    symbols[identifier] = value;
}

Context::Context(Assembler* assembler) : assembler{assembler}, section{0, true} {

}

