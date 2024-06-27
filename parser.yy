%skeleton "lalr1.cc"
%require "3.8.2"
%header

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert


%code requires {
    #include "Expression.hpp"
    #include "Statement.hpp"
    #include "InstructionStatement.hpp"
    #include "MacroStatement.hpp"
    #include <SpdrFirmware/Register.hpp>
    #include <SpdrFirmware/Mode.hpp>
    #include <string>
    #include <cstdint>
    #include <memory>
    #include <vector>
    #include <cstdio>
    #include <tuple>
    class Driver;
    class Block;

    extern std::FILE* yyin;
}

%param { Driver& driver }

%locations
%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
    #include <iostream>
    #include "Error.hpp"
    #include "Driver.hpp"
}

%define api.token.prefix {TOK_}

%token ENDLINE "newline" COLON ":" 
    LPAREN "(" RPAREN ")" 
    LBRACKET "[" RBRACKET "]"
    PLUS "+"
    MINUS "-"
    ASTRIX "*"
    SLASH "/"
    PERCENT "%"
    AND "&"
    OR "|"
    XOR "^"
    NOT "!"
    INVERT "~"
    ASSIGN "="
    LESS "<"
    GREATER ">"
    DOT "."
    COMMA ","
    GREATER_EQUAL ">="
    LESS_EQUAL "<="
    EQUAL "=="
    NOT_EQUAL "!="
    LOG_AND "&&"
    LOG_OR "||"
    LEFT_SHIFT "<<"
    RIGHT_SHIFT ">>"
    ;

%token
    INCLUDE "include"
    INCLUDE_BIN "include_bin"
    ADDRESS "address"
    ALIGN "align"
    RES "res"
    RESW "resw"
    BYTE "byte"
    BYTES "bytes"
    WORD "word"
    SECTION "section"
    DATA "data"
    DATAW "dataw"
    ONCE "once"
    MACRO "macro"
    ENDMACRO "endmacro"
    VARIABLE "variable"
    PROVIDES "provides"
    IF "if"
    ELSE "else"
    ELSEIF "elseif"
    REPEAT "repeat"
    END "end"
    NAMESPACE "namespace"
    ;

%token A C D CD F SP

%token <UnqualifiedIdentifier> UID "raw_unqualified_id"
%token <std::string> IDENTIFIER "identifier" STRING "string";
%token <std::int64_t> INTEGER "integer";

%%

%start target;

target
    : statements {driver.parsed = $1;}
    ;

%nterm <Block*> statements;
statements
    : statements_nonempty {$$ = $1;}
    | %empty {$$ = new Block{};}
    ;

%nterm <Block*> statements_nonempty;
statements_nonempty
    : statements_nonempty complete_statement {if ($2) $1->push($2); $$ = $1;}
    | complete_statement {$$ = new Block{}; if ($1) $$->push($1);}
    | "once" newline {$$ = new Block{}; $$->once = true;}
    | statements_nonempty "once" newline {$$ = $1; $$->once = true;}
    ;

%nterm <Statement*> complete_statement;
complete_statement
    : statement
    | error newline {$$ = nullptr;}
    ;

%nterm <Statement*> statement;
statement
    : label
    | statement_endline newline {$$ = $1;}
    | newline {$$ = nullptr;}
    ;

%nterm <Statement*> statement_endline;
statement_endline
    : instruction
    | symbol
    | "section" STRING {$$ = new SectionStatement{@$, $2};}
    | "address" expression {$$ = new AddressStatement{@$, $2};}
    | "align" expression {$$ = new AlignStatement{@$, $2};}
    | "res" expression {$$ = new ReserveStatement{@$, $2};}
    | "data" data_element_list {$$ = new DataStatement(@$, $2, 1);}
    | "dataw" data_element_list {$$ = new DataStatement(@$, $2, 2);}
    | "include" STRING {$$ = new IncludeStatement(@$, IncludeStatement::Type::Assembly, $2);}
    | "include_bin" STRING {$$ = new IncludeStatement(@$, IncludeStatement::Type::Binary, $2);}
    | macro_statement
    | VARIABLE ident expression {$$ = new VariableStatement(@$, $2, $3);}
    | "provides" STRING {$$ = new ProvidesStatement(@$, $2);}
    | conditional_assembly
    | repeat_block
    ;

%nterm <Statement*> conditional_assembly;
conditional_assembly
    : "if" expression newline statements else_conditional
        {$$ = new ConditionalStatement{@1, $2, $4, $5};}
    ;

%nterm <std::optional<Block*>> else_conditional;
else_conditional
    : "else" newline statements "end" {$$ = {$3};}
    | "end" {$$ = std::nullopt;}
    | "elseif" expression newline statements else_conditional
        {$$ = new Block{{new ConditionalStatement{@1, $2, $4, $5}}};}
    ;

%nterm <Statement*> repeat_block;
repeat_block
    : "repeat" IDENTIFIER "," expression newline statements "end"
        {$$ = new RepeatStatement{@$, $4, $6, $2};}
    | "repeat" expression newline statements "end"
        {$$ = new RepeatStatement{@$, $2, $4};}
    ;

%nterm <Statement*> macro_statement;
macro_statement
    : "macro" IDENTIFIER parameter_list newline statements "endmacro"
        { $$ = new MacroStatement(@$, $2, $3, $5); }
    | "macro" IDENTIFIER newline statements "endmacro"
        { $$ = new MacroStatement(@$, $2, {}, $4); }
    ;

%nterm <std::vector<DataElement*>> data_element_list;
data_element_list
    : data_element {$$ = {$1};}
    | data_element_list "," data_element {$$ = $1; $$.push_back($3);}
    ;

%nterm <DataElement*> data_element;
data_element
    : STRING {$$ = new StringElement(@$, $1);}
    | expression {$$ = new ExpressionElement(@$, $1);}
    | data_size expression {$$ = new ExpressionElement(@$, $2, $1);}
    ;

%nterm <int> data_size;
data_size
    : "byte" {$$ = 1;}
    | "word" {$$ = 2;}
    | "bytes" "(" INTEGER ")" {$$ = $3;}
    ;

%nterm <Statement*> instruction;
instruction
    : IDENTIFIER {$$ = new InstructionStatement{@$, $1, {}};}
    | IDENTIFIER mode_list {$$ = new InstructionStatement{@$, $1, std::move($2)};}
    ;

%nterm <std::vector<std::pair<Address, Expression*>>> mode_list;
mode_list
    : mode {$$ = std::vector<std::pair<Address, Expression*>>{}; $$.push_back($1);}
    | mode_list "," mode {$1.push_back($3); $$ = std::move($1);}
    ;

%nterm <std::pair<Address, Expression*>> mode;
mode
    : reg         {$$ = {$1, nullptr};}
    | expression  {$$ = {Mode::Immediate, $1};}
    | "[" reg "]" {$$ = {{Mode::Indirect, $2}, nullptr};}
    | "[" expression "]" {$$ = {Mode::Direct, $2};}
    | "[" reg "," expression "]" {$$ = {{Mode::Offset, $2}, $4};}
    ;

%nterm <std::vector<std::pair<Address, std::optional<std::string>>>> parameter_list;
parameter_list
    : parameter_mode {$$ = {}; $$.push_back($1);}
    | parameter_list "," parameter_mode {$$ = std::move($1); $$.push_back($3);}
    ;

%nterm <std::pair<Address, std::optional<std::string>>> parameter_mode;
parameter_mode
    : reg         {$$ = {$1, {}};}
    | IDENTIFIER {$$ = {Mode::Immediate, $1};}
    | "[" reg "]" {$$ = {{Mode::Indirect, $2}, {}};}
    | "[" IDENTIFIER "]" {$$ = {Mode::Direct, $2};}
    | "[" reg "," IDENTIFIER "]" {$$ = {{Mode::Offset, $2}, $4};}
    ;

newline
    : ENDLINE
    ;

%nterm <RID> reg;
reg
    : A  {$$ = RID::A;}
    | C  {$$ = RID::C;}
    | D  {$$ = RID::D;}
    | CD {$$ = RID::CD;}
    | F  {$$ = RID::F;}
    | SP {$$ = RID::Sp;}
    ;

%nterm <Statement*> label;
label
    : ident ":" {$$ = new LabelStatement{@$, $1};}
    ;

%nterm <Statement*> symbol;
symbol
    : ident "=" expression {$$ = new SymbolStatement{@$, $1, $3};}
    ;

%nterm <UnqualifiedIdentifier> ident;
ident
    : UID {$$ = $1;} 
    | IDENTIFIER {$$ = UnqualifiedIdentifier(0, {$1});}
    ;


%left "||";
%left "&&";
%left "|";
%left "^";
%left "&";
%left "==" "!=";
%left "<=" ">=" "<" ">";
%left "<<" ">>";
%left "+" "-";
%left "*" "/" "%";
%left "~" "!";

%nterm <Expression*> expression;
expression
    : INTEGER {$$ = new LiteralExpression{@$, $1};}
    | expression "+" expression {$$ = new BinaryExpression{@$, Binary::Add, $1, $3};}
    | expression "-" expression {$$ = new BinaryExpression{@$, Binary::Subtract, $1, $3};}
    | expression "*" expression {$$ = new BinaryExpression{@$, Binary::Multiply, $1, $3};}
    | expression "/" expression {$$ = new BinaryExpression{@$, Binary::Divide, $1, $3};}
    | expression "%" expression {$$ = new BinaryExpression{@$, Binary::Modulo, $1, $3};}
    | "-" expression %prec "~" {$$ = new UnaryExpression{@$, Unary::Negate, $2};}
    | expression "<<" expression {$$ = new BinaryExpression{@$, Binary::ShiftLeft, $1, $3};}
    | expression ">>" expression {$$ = new BinaryExpression{@$, Binary::ShiftRight, $1, $3};}
    | expression "&" expression {$$ = new BinaryExpression{@$, Binary::BinAnd, $1, $3};}
    | expression "|" expression {$$ = new BinaryExpression{@$, Binary::BinOr, $1, $3};}
    | expression "^" expression {$$ = new BinaryExpression{@$, Binary::BinXor, $1, $3};}
    | "~" expression            {$$ = new UnaryExpression{@$, Unary::BinNot, $2};}
    | expression "&&" expression {$$ = new BinaryExpression{@$, Binary::And, $1, $3};}
    | expression "||" expression {$$ = new BinaryExpression{@$, Binary::Or, $1, $3};}
    | "!" expression            {$$ = new UnaryExpression{@$, Unary::Not, $2};}
    | expression ">" expression {$$ = new BinaryExpression{@$, Binary::Greater, $1, $3};}
    | expression "<" expression {$$ = new BinaryExpression{@$, Binary::Less, $1, $3};}
    | expression ">=" expression {$$ = new BinaryExpression{@$, Binary::GreaterEqual, $1, $3};}
    | expression "<=" expression {$$ = new BinaryExpression{@$, Binary::LessEqual, $1, $3};}
    | expression "==" expression {$$ = new BinaryExpression{@$, Binary::Equal, $1, $3};}
    | expression "!=" expression {$$ = new BinaryExpression{@$, Binary::NotEqual, $1, $3};}
    | "(" expression ")" {$$ = $2;}
    | ident {$$ = new SymbolicExpression{@$, $1};}
    ;

%%

void yy::parser::error(const location_type& loc, const std::string& message) {
    std::clog << Error{Error::Level::Fatal, loc, message};
}

