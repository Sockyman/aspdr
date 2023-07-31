#include "Section.hpp"
#include "Assembler.hpp"
#include <stdexcept>

Section::Section(std::size_t start, bool isWritable)
: bytes{}, start{start}, offset{0}, writable{isWritable}, counting{true} {
}

VoidResult Section::writeInteger(const Location& location,
    Result<std::int64_t> value,
    int number
) {
    if (!this->writable) {
        return value.then(VoidResult{{Error::Level::Fatal, location,
            "section is not writable"}});
    }

    if (!this->counting) {
        return value.then(VoidResult{});
    }

    this->offset += number;

    if (value.isErr()) {
        return value.into<VoidResult>();
    }

    for (int i = 0; i < number; ++i) {
        this->bytes.push_back((value.getOk() >> (i * 8)) & 0xff);
    }
    return value.then(VoidResult{});
}

VoidResult Section::writeInteger(const Context& context,
    const Expression* expr,
    int number
) {
    return this->writeInteger(expr->location, expr->evaluate(context), number);
}

VoidResult Section::reserve(const Location& location, std::int64_t number) {
    if (number < 0) {
        return {{Error::Level::Fatal, location, "reserve must be positive"}};
    }

    if (!this->counting)
        return VoidResult{};

    this->offset += number;
    if (this->writable) {
        this->bytes.resize(this->bytes.size() + number, 0);
    }
    return VoidResult{};
}

VoidResult Section::reserve(const Context& context, const Expression* expr) {
    return this->evaluateAndRun(context, expr, [this, expr](std::int64_t x){
        return this->reserve(expr->location, x);
    });
}

VoidResult Section::changeAddress(
    const Location& location,
    std::int64_t address
) {
    if (!this->counting)
        return VoidResult{};

    if (*this->getAddress() > address) {
        return VoidResult{{Error::Level::Fatal, location,
            "new address before previous address"}}; // TODO: Location
    }
    return this->reserve(location, address - *this->getAddress());
}

VoidResult Section::changeAddress(const Context& context, const Expression* expr) {
    return this->evaluateAndRun(context, expr, [this, expr](std::int64_t x){
        return this->changeAddress(expr->location, x);
    });
}

std::optional<std::int64_t> Section::getAddress() {
    if (!this->counting)
        return {};

    return this->offset + this->start;
}


VoidResult Section::writeByte(const Location& location, std::uint8_t value) {
    return this->writeInteger(location, value, 1);
}

VoidResult Section::writeByte(const Context& context, const Expression* expr) {
    return this->writeInteger(context, expr, 1);
}

VoidResult Section::writeWord(const Location& location, std::int64_t value) {
    return this->writeInteger(location, value, 2);
}

VoidResult Section::writeWord(const Context& context, const Expression* expr) {
    return this->writeInteger(context, expr, 2);
}

