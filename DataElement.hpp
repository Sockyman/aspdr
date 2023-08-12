#ifndef DATAELEMENT_HPP
#define DATAELEMENT_HPP

#include "Result.hpp"
#include "Location.hpp"
#include "Expression.hpp"
#include <vector>
#include <string>
#include <optional>

class Context;

class DataElement {
public:
    const Location location;

    DataElement(Location location);

    virtual VoidResult write(Context& context, int defaultSize) = 0;
};

class ExpressionElement : public DataElement {
public:
    Expression* expression;
    std::optional<int> size;

    ExpressionElement(Location location, Expression* expr, std::optional<int> size = {});

    virtual VoidResult write(Context& context, int defaultSize) override;
};

class StringElement : public DataElement {
public:
    std::vector<char> data;

    StringElement(Location location, std::string str);
    StringElement(Location location, std::vector<char> data);

    virtual VoidResult write(Context& context, int defaultSize) override;
};

#endif

