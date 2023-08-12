#include "Section.hpp"
#include "Assembler.hpp"
#include "Context.hpp"
#include <stdexcept>
#include <sstream>

Section::Section() {}

Section::Section(SectionInfo* sectionInfo)
: bytes{}, offset{0}, sectionInfo{sectionInfo} {
}

VoidResult Section::assertWritable(Context& context, const Location& location) const {
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
    int number
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
        this->bytes.push_back((value.getOk() >> (i * 8)) & 0xff);
    }
    return VoidResult{};
}

VoidResult Section::writeInteger(
    Context& context,
    const Expression* expr,
    int number
) {
    return this->writeInteger(context, expr->location, expr->evaluate(context), number);
}

VoidResult Section::writeBytes(Context& context, const Location& location, const std::vector<char>& bytes) {
    auto isWritable = this->assertWritable(context, location);
    if (isWritable.isErr() || !this->offset) {
        return isWritable;
    }
    *this->offset += bytes.size();
    this->bytes.insert(this->bytes.end(), bytes.begin(), bytes.end());
    return VoidResult{};
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
        return context.voidError({Error::Level::Fatal, location, "reserve must be positive"});
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
        return context.voidError({Error::Level::Fatal, location,
            "new address before previous address"}); // TODO: Location
    }
    return this->reserve(context, location, address.getOk() - *this->getAddress());
}

VoidResult Section::changeAddress(
    Context& context,
    const Expression* expr
) {
    return this->changeAddress(context, expr->location, expr->evaluate(context));
}

VoidResult Section::align(Context& context, const Location& location, Result<std::int64_t> number) {
    if (number.isErr()) {
        return number.intoVoid();
    }

    auto address = this->getAddress();
    if (!address) {
        return VoidResult{};
    }

    auto reservation = *number - (*address % *number);
    // std::cout << "address: " << *address << ", alignment: " << *number << ", reserve: " << reservation << "\n";
    if (reservation < 0) {
        return context.voidError({Error::Level::Fatal, location, "align must be positive"});
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

