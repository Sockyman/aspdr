#ifndef SECTION_HPP
#define SECTION_HPP

#include "Result.hpp"
#include "Location.hpp"
#include "Expression.hpp"
#include "SectionInfo.hpp"
#include <cstdint>
#include <vector>
#include <optional>

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
        int number
    );

    VoidResult writeInteger(
        Context& context,
        const Expression* expr,
        int number
    );

    VoidResult writeBytes(Context& context, const Location& location, const std::vector<char>& bytes);

    VoidResult writeByte(Context& context, const Location& location, Result<std::int64_t> value);
    VoidResult writeByte(Context& context, const Expression* expr);

    VoidResult writeWord(Context& context, const Location& location, Result<std::int64_t> value);
    VoidResult writeWord(Context& context, const Expression* expr);

    VoidResult reserve(Context& context, const Location& location, Result<std::int64_t> number);
    VoidResult reserve(Context& context, const Expression* expr);


    VoidResult changeAddress(
        Context& context,
        const Location& location,
        Result<std::int64_t> address
    );

    VoidResult changeAddress(Context& context, const Expression* expr);

    VoidResult align(Context& context, const Location& location, Result<std::int64_t> number);
    VoidResult align(Context& context, const Expression* expr);

    std::optional<std::int64_t> getAddress();

    VoidResult assertWritable(Context& context, const Location& location) const;
};

#endif

