#include "DataElement.hpp"
#include "stringliteral.hpp"
#include "Context.hpp"

DataElement::DataElement(Location location) : location{location} {}

DataElement::~DataElement() {}


ExpressionElement::ExpressionElement(
    Location location,
    Expression* expr,
    std::optional<int> size
) : DataElement{location}, expression{expr}, size{size} {}

VoidResult ExpressionElement::write(Context& context, int defaultSize) {
    if (this->size) {
        defaultSize = *this->size;
    }
    return context
        .getSection()
        .writeInteger(context, this->expression, defaultSize);
}

ExpressionElement::~ExpressionElement() {
    delete this->expression;
}


StringElement::StringElement(Location location, std::string str)
: DataElement{location}, data{processEscapedString(str)} {}

StringElement::StringElement(Location location, std::vector<char> data)
: DataElement{location}, data{data} {}

VoidResult StringElement::write(Context& context, int defaultSize) {
    return context.getSection().writeBytes(context, this->location, this->data);
}

