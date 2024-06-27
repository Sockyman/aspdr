#ifndef MACROSTATEMENT_HPP
#define MACROSTATEMENT_HPP

#include "Statement.hpp"
#include <SpdrFirmware/Mode.hpp>
#include <string>

class MacroStatement : public Statement {
public:
    std::string name;
    std::vector<std::pair<Address, std::optional<std::string>>> parameters;
    Block* block;

    MacroStatement(
        Location location,
        std::string name,
        std::vector<std::pair<Address, std::optional<std::string>>> parameters,
        Block* block
    );

    virtual ~MacroStatement() override;

    virtual bool assemble(Context& context) override;
    bool assembleBlock(Context& context, const std::vector<Expression*>& arguments, int id);

    Instruction getInstruction();
};

#endif

