#ifndef SECTION_HPP
#define SECTION_HPP

#include "Result.hpp"
#include "Location.hpp"
#include "Expression.hpp"
#include <cstdint>
#include <vector>
#include <optional>

class Context;

class Section {
private:
    template<typename F>
    VoidResult evaluateAndRun(
        const Context& context,
        const Expression* expr,
        const F& func);
public:
    std::vector<std::uint8_t> bytes;
    std::size_t start;
    std::size_t offset;

    bool writable;
    bool counting;

    Section(std::size_t start, bool isWritable);

    VoidResult writeInteger(
        const Location& location,
        Result<std::int64_t> value,
        int number);

    VoidResult writeInteger(
        const Context& context,
        const Expression* expr,
        int number);

    VoidResult writeByte(const Location& location, std::uint8_t value);
    VoidResult writeByte(const Context& context, const Expression* expr);

    VoidResult writeWord(const Location& location, std::int64_t value);
    VoidResult writeWord(const Context& context, const Expression* expr);

    VoidResult reserve(const Location& location, std::int64_t number);
    VoidResult reserve(const Context& context, const Expression* expr);

    VoidResult changeAddress(const Location& location, std::int64_t address);
    VoidResult changeAddress(const Context& context, const Expression* expr);

    std::optional<std::int64_t> getAddress();
};

template<typename F>
VoidResult Section::evaluateAndRun(
    const Context& context,
    const Expression* expr,
    const F& func
) {
    Result<std::int64_t> result = expr->evaluate(context);
    if (result.isErr()) {
        return result.into<VoidResult>();
    }
    return func(result.getOk());
}

#endif

