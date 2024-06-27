#include "Section.hpp"
#include "Assembler.hpp"
#include "Context.hpp"
#include "Error.hpp"
#include "InstructionStatement.hpp"
#include <sstream>
#include <format>

Section::Section() {}

Section::Section(SectionInfo* sectionInfo)
: offset{0}, bytes{}, sectionInfo{sectionInfo} {
}

bool Section::assertWritable(
    Context& context,
    const Location& location
) const {
    if (!this->sectionInfo->writable) {
        std::stringstream ss{};
        ss << "section \'" << this->sectionInfo->name << "\' is not writable";
        context.error(Error::Level::Fatal, ss.str(), location);
        return false;
    }
    return true;
}

bool Section::writeInteger(
    Context& context,
    const Location& location,
    std::optional<std::int64_t> value,
    int number,
    int shift
) {
    auto isWritable = this->assertWritable(context, location);
    if (!isWritable || !this->offset) {
        return false;
    }

    *this->offset += number;

    if (!value.has_value()) {
        return false;
    }

    for (int i = 0; i < number; ++i) {
        this->bytes.push_back((value.value() >> ((i + shift) * 8)) & 0xff);
    }
    return true;
}

bool Section::writeInteger(
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

bool Section::writeBytes(
    Context& context,
    const Location& location,
    const std::vector<char>& bytes
) {
    auto isWritable = this->assertWritable(context, location);
    if (!isWritable || !this->offset) {
        return isWritable;
    }
    *this->offset += bytes.size();
    this->bytes.insert(this->bytes.end(), bytes.begin(), bytes.end());
    return true;
}

bool Section::writeAddress(
    Context& context,
    Size size,
    const Expression* expr
) {
    auto& section = context.getSection();

    switch (size) {
        case Size::Unsized:
            return true;
        case Size::Word:
            return section.writeInteger(context, expr, 2);
        case Size::Byte:
            return section.writeInteger(context, expr, 1);
        case Size::Page:
            return section.writeInteger(context, expr, 1, 1);
    }
    UNREACHABLE;
}

bool Section::writeInstruction(
    Context& context,
    const InstructionStatement* statement
) {
    /*if (ins.name == "call") {
        this->writeInstruction(context, location, {"phc", {}}, {});
        this->writeInstruction(context, location, {"jmp", ins.mode}, expressions);
        if (ins.mode.destination.address.mode == Mode::Register) {
            this->writeInstruction(context, location, {"nop", {}}, {});
            this->writeInstruction(context, location, {"nop", {}}, {});
        }
        return true;
    }*/

    const Instruction& ins{statement->instruction};

    auto micros = context.assembler->instructionSet.getInstruction(ins);

    if (!micros && context.macros.contains(ins)) {
        return context.macros[ins]->assembleBlock(
            context,
            statement->arguments,
            statement->statementId
        );
    }

    if (!micros) {
        std::stringstream ss{};
        ss << "\'" << ins << "\' is not an instruction or macro";
        context.error(Error::Level::Fatal, ss.str(), statement->location);
        return false;
    }

    auto opcode = context.assembler->instructionSet.getOpcode(ins);

    bool result = this->writeByte(context, statement->location, *opcode);

    const std::vector<SizedAddress>& mode = micros->instruction.mode.mode;
    for (std::size_t i = 0; i < mode.size(); ++i) {
        result = result
            && this->writeAddress(context, mode[i].size, statement->arguments[i]);
    }

    /*bool firstResult = this->writeAddress(
            context,
            (*micros)->instruction.mode.source.size,
            expressions[1]);
    bool secondResult = this->writeAddress(
            context,
            (*micros)->instruction.mode.destination.size,
            expressions[0]);*/

    return result;
}

bool Section::reserve(
    Context& context,
    const Location& location,
    std::optional<std::int64_t> number
) {
    if (!number.has_value()) {
        this->offset = {};
        return false;
    }

    if (number.value() < 0) {
        context.error(
            Error::Level::Fatal, "reserve must be positive", location
        );
        return false;
    }

    if (!this->offset) {
        return true;
    }

    *this->offset += number.value();
    if (this->sectionInfo->writable) {
        this->bytes.resize(this->bytes.size() + number.value(), 0);
    }
    return true;
}

bool Section::reserve(
    Context& context, 
    const Expression* expr
) {
    return this->reserve(context, expr->location, expr->evaluate(context));
}

bool Section::changeAddress(
    Context& context,
    const Location& location,
    std::optional<std::int64_t> address
) {
    if (!address.has_value()) {
        this->offset = {};
        return false;
    }

    if (!this->offset) {
        return true;
    }

    if (*this->getAddress() > address.value()) {
        auto message = std::format(
            "new address '0x{:04x}' before previous address '0x{:04x}'",
            address.value(),
            *this->getAddress()
        );

        context.error(
            Error::Level::Fatal,
            message,
            location
        );
        return false;
    }
    return this->reserve(
        context,
        location,
        address.value() - *this->getAddress()
    );
}

bool Section::changeAddress(
    Context& context,
    const Expression* expr
) {
    return this->changeAddress(context, expr->location, expr->evaluate(context));
}

bool Section::align(
    Context& context,
    const Location& location,
    std::optional<std::int64_t> number
) {
    if (!number.has_value()) {
        return false;
    }

    auto address = this->getAddress();
    if (!address) {
        return true;
    }

    if (*address % *number == 0) {
        return true;
    }

    auto reservation = *number - (*address % *number);

    if (reservation < 0) {
        context.error(
            {Error::Level::Fatal, location, "align must be positive"}
        );
        return false;
    }
    return this->reserve(context, location, reservation);
}

bool Section::align(Context& context, const Expression* expr) {
    return this->align(context, expr->location, expr->evaluate(context));
}

std::optional<std::int64_t> Section::getAddress() {
    if (!this->offset) {
        return {};
    }

    return *this->offset + this->sectionInfo->start;
}


bool Section::writeByte(
    Context& context,
    const Location& location,
    std::optional<std::int64_t> value
) {
    return this->writeInteger(context, location, value, 1);
}

bool Section::writeByte(Context& context, const Expression* expr) {
    return this->writeInteger(context, expr, 1);
}

bool Section::writeWord(
    Context& context,
    const Location& location,
    std::optional<std::int64_t> value
) {
    return this->writeInteger(context, location, value, 2);
}

bool Section::writeWord(Context& context, const Expression* expr) {
    return this->writeInteger(context, expr, 2);
}

std::span<const char> Section::getBytes() const {
    return this->bytes;
}

