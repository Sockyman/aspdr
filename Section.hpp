#ifndef SECTION_HPP
#define SECTION_HPP

#include "Result.hpp"
#include "Location.hpp"
#include "Expression.hpp"
#include "SectionInfo.hpp"
#include <SpdrFirmware/Instruction.hpp>
#include <cstdint>
#include <vector>
#include <optional>
#include <span>

class Context;

class Section {
private:
public:
    std::vector<char> bytes;
    std::optional<std::int64_t> offset;

    SectionInfo* sectionInfo;

    Section();
    Section(SectionInfo* sectionInfo);

    VoidResult writeInteger(
        Context& context,
        const Location& location,
        Result<std::int64_t> value,
        int number,
        int shift = 0
    );

    VoidResult writeInteger(
        Context& context,
        const Expression* expr,
        int number,
        int shift = 0
    );

    VoidResult writeBytes(
        Context& context,
        const Location& location,
        const std::vector<char>& bytes
    );

    VoidResult writeByte(
        Context& context,
        const Location& location,
        Result<std::int64_t> value
    );

    VoidResult writeByte(Context& context, const Expression* expr);

    VoidResult writeWord(
        Context& context, 
        const Location& location,
        Result<std::int64_t> value
    );

    VoidResult writeWord(Context& context, const Expression* expr);

    VoidResult writeAddress(
        Context& context,
        Size size,
        const Expression* expr
    );

    VoidResult writeInstruction(
        Context& context,
        const Location& location,
        const Instruction& ins,
        const std::array<Expression*, 2>& expressions
    );

    VoidResult reserve(
        Context& context,
        const Location& location,
        Result<std::int64_t> number
    );

    VoidResult reserve(Context& context, const Expression* expr);


    VoidResult changeAddress(
        Context& context,
        const Location& location,
        Result<std::int64_t> address
    );

    VoidResult changeAddress(Context& context, const Expression* expr);

    VoidResult align(
        Context& context,
        const Location& location,
        Result<std::int64_t> number
    );

    VoidResult align(Context& context, const Expression* expr);

    std::optional<std::int64_t> getAddress();

    VoidResult assertWritable(Context& context, const Location& location) const;
};

#endif

