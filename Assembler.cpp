#include "Assembler.hpp"
#include "Error.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <vector>

Assembler::Assembler(std::vector<Statement*> statements)
: statements{statements}, instructionSet{} {}

Result<Context> Assembler::pass() {
    ErrorList errors{};

    Context context{this};
    for (auto statement : statements) {
        VoidResult result = statement->assemble(context);

        if (result.isErr()) {
            errors.insert(result.getErr());
        }
    }

    if (errors.hasErrors()) {
        return Result<Context>{errors};
    }
    return Result{context};
}

Result<Context> Assembler::passes() {
    const int maxPasses = 2;
    ErrorList previousErrors{};

    for (int i = 0; i < maxPasses; ++i) {
        Result result = this->pass();

        if (result.isOk()) {
            return result;
        }

        if (previousErrors == result.getErr()) {
            return result;
        }
        previousErrors = std::move(result.getErr());
    }
    return Result<Context>{previousErrors};
}

void Assembler::assemble() {
    auto result{passes()};

    if (result.isErr()) {
        result.getErr().display(std::clog);
        return;
    }

    for (auto b : result.getOk().section.bytes) {
        std::printf("%02x ", b);
    }
    std::cout << '\n';
}

std::optional<std::int64_t> Assembler::resolveSymbol(
    const Identifier& identifier
) const {
    if (!this->symbols.contains(identifier))
        return {};
    return {this->symbols.at(identifier)};
}

VoidResult Assembler::assignSymbol(const Location& location, const Identifier& identifier, std::int64_t value) {
    //std::cout << identifier << ": " << value << '\n';
    auto existing = this->resolveSymbol(identifier);
    if (existing && *existing != value) {
        std::stringstream ss{};
        ss << "redefinition of \'" << identifier << "\'";
        return {{Error::Level::Fatal, location, ss.str()}};
    }
    this->symbols[identifier] = value;
    return VoidResult{};
}


Context::Context(Assembler* assembler)
:   assembler{assembler}, section{0, true} {}

