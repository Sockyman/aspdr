#include "Section.hpp"
#include "Assembler.hpp"
#include "Context.hpp"
#include <stdexcept>
#include <sstream>
#include <format>

Section::Section() {}

Section::Section(SectionInfo* sectionInfo)
: bytes{}, offset{0}, sectionInfo{sectionInfo} {
}

VoidResult Section::assertWritable(
    Context& context,
    const Location& location
) const {
    if (!this->sectionInfo->writable) {
        std::stringstream ss{};
        ss << "section \'" << this->sectionInfo->name << "\' is not writable";
        return context.voidError({Error::Level::Fatal, location, ss.str()});
    }
    return VoidResult{};
}

VoidResult Section::writeInteger(
    Context& context,
    const Location& location,
    Result<std::int64_t> value,
    int number,
    int shift
) {
    auto isWritable = this->assertWritable(context, location);
    if (isWritable.isErr() || !this->offset) {
        return isWritable;
    }

    *this->offset += number;

    if (value.isErr()) {
        return value.intoVoid();
    }

    for (int i = 0; i < number; ++i) {
        this->bytes.push_back((value.getOk() >> ((i + shift) * 8)) & 0xff);
    }
    return VoidResult{};
}

VoidResult Section::writeInteger(
    Context& context,
    const Expression* expr,
    int number,
    int shift
) {
    return this->writeInteger(
        context,
        expr->location,
        expr->evaluate(context),
        number,
        shift
    );
}

VoidResult Section::writeBytes(
    Context& context,
    const Location& location,
    const std::vector<char>& bytes
) {
    auto isWritable = this->assertWritable(context, location);
    if (isWritable.isErr() || !this->offset) {
        return isWritable;
    }
    *this->offset += bytes.size();
    this->bytes.insert(this->bytes.end(), bytes.begin(), bytes.end());
    return VoidResult{};
}

VoidResult Section::writeAddress(
    Context& context,
    Size size,
    const Expression* expr
) {
    auto& section = context.getSection();

    switch (size) {
        case Size::Unsized:
            return VoidResult{};
        case Size::Word:
            return section.writeInteger(context, expr, 2);
        case Size::Byte:
            return section.writeInteger(context, expr, 1);
        case Size::Page:
            return section.writeInteger(context, expr, 1, 1);
    }
    UNREACHABLE;
}

VoidResult Section::writeInstruction(
    Context& context,
    const Location& location,
    const Instruction& ins,
    const std::array<Expression*, 2>& expressions
) {
    auto micros = context.assembler->instructionSet.getInstruction(ins);

    if (ins.name == "call") {
        this->writeInstruction(context, location, {"phc", {}}, {});
        this->writeInstruction(context, location, {"jmp", ins.mode}, expressions);
        if (ins.mode.destination.address.mode == Mode::Register) {
            this->writeInstruction(context, location, {"nop", {}}, {});
            this->writeInstruction(context, location, {"nop", {}}, {});
        }
        return VoidResult{};
    }

    if (!micros) {
        std::stringstream ss{};
        ss << "instruction \'" << ins << "\' does not exist";
        return context.voidError({Error::Level::Fatal, location, ss.str()});
    }

    auto opcode = context.assembler->instructionSet.getOpcode(ins);

    return context.getSection().writeByte(context, location, *opcode)
        .then(this->writeAddress(
            context,
            (*micros)->instruction.mode.source.size,
            expressions[1]))
        .then(this->writeAddress(
            context,
            (*micros)->instruction.mode.destination.size,
            expressions[0]));
}

VoidResult Section::reserve(
    Context& context,
    const Location& location,
    Result<std::int64_t> number
) {
    if (number.isErr()) {
        this->offset = {};
        return number.intoVoid();
    }

    if (number.getOk() < 0) {
        return context.voidError(
            {Error::Level::Fatal, location, "reserve must be positive"}
        );
    }

    if (!this->offset) {
        return VoidResult{};
    }

    *this->offset += number.getOk();
    if (this->sectionInfo->writable) {
        this->bytes.resize(this->bytes.size() + number.getOk(), 0);
    }
    return VoidResult{};
}

VoidResult Section::reserve(
    Context& context, 
    const Expression* expr
) {
    return this->reserve(context, expr->location, expr->evaluate(context));
}

VoidResult Section::changeAddress(
    Context& context,
    const Location& location,
    Result<std::int64_t> address
) {
    if (address.isErr()) {
        this->offset = {};
        return address.into<std::monostate>();
    }

    if (!this->offset) {
        return VoidResult{};
    }

    if (*this->getAddress() > address.getOk()) {
        auto message = std::format(
            "new address '0x{:04x}' before previous address '0x{:04x}'",
            address.getOk(),
            *this->getAddress()
        );

        return context.voidError({
            Error::Level::Fatal,
            location,
            message
        });
    }
    return this->reserve(
        context,
        location,
        address.getOk() - *this->getAddress()
    );
}

VoidResult Section::changeAddress(
    Context& context,
    const Expression* expr
) {
    return this->changeAddress(context, expr->location, expr->evaluate(context));
}

VoidResult Section::align(
    Context& context,
    const Location& location,
    Result<std::int64_t> number
) {
    if (number.isErr()) {
        return number.intoVoid();
    }

    auto address = this->getAddress();
    if (!address) {
        return VoidResult{};
    }

    if (*address % *number == 0) {
        return VoidResult{};
    }

    auto reservation = *number - (*address % *number);

    if (reservation < 0) {
        return context.voidError(
            {Error::Level::Fatal, location, "align must be positive"}
        );
    }
    return this->reserve(context, location, reservation);
}

VoidResult Section::align(Context& context, const Expression* expr) {
    return this->align(context, expr->location, expr->evaluate(context));
}

std::optional<std::int64_t> Section::getAddress() {
    if (!this->offset)
        return {};

    return *this->offset + this->sectionInfo->start;
}


VoidResult Section::writeByte(
    Context& context,
    const Location& location,
    Result<std::int64_t> value
) {
    return this->writeInteger(context, location, value, 1);
}

VoidResult Section::writeByte(Context& context, const Expression* expr) {
    return this->writeInteger(context, expr, 1);
}

VoidResult Section::writeWord(
    Context& context,
    const Location& location,
    Result<std::int64_t> value
) {
    return this->writeInteger(context, location, value, 2);
}

VoidResult Section::writeWord(Context& context, const Expression* expr) {
    return this->writeInteger(context, expr, 2);
}

