#ifndef SECTION_HPP
#define SECTION_HPP

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

    bool writeInteger(
        Context& context,
        const Location& location,
        std::optional<std::int64_t> value,
        int number,
        int shift = 0
    );

    bool writeInteger(
        Context& context,
        const Expression* expr,
        int number,
        int shift = 0
    );

    bool writeBytes(
        Context& context,
        const Location& location,
        const std::vector<char>& bytes
    );

    bool writeByte(
        Context& context,
        const Location& location,
        std::optional<std::int64_t> value
    );

    bool writeByte(Context& context, const Expression* expr);

    bool writeWord(
        Context& context, 
        const Location& location,
        std::optional<std::int64_t> value
    );

    bool writeWord(Context& context, const Expression* expr);

    bool writeAddress(
        Context& context,
        Size size,
        const Expression* expr
    );

    bool writeInstruction(
        Context& context,
        const Location& location,
        const Instruction& ins,
        const std::array<Expression*, 2>& expressions
    );

    bool reserve(
        Context& context,
        const Location& location,
        std::optional<std::int64_t> number
    );

    bool reserve(Context& context, const Expression* expr);


    bool changeAddress(
        Context& context,
        const Location& location,
        std::optional<std::int64_t> address
    );

    bool changeAddress(Context& context, const Expression* expr);

    bool align(
        Context& context,
        const Location& location,
        std::optional<std::int64_t> number
    );

    bool align(Context& context, const Expression* expr);

    std::optional<std::int64_t> getAddress();

    bool assertWritable(Context& context, const Location& location) const;
};

#endif

