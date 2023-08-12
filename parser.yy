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
    #include <SpdrFirmware/Register.hpp>
    #include <SpdrFirmware/Mode.hpp>
    #include <string>
    #include <cstdint>
    #include <memory>
    #include <tuple>
    #include <vector>
    #include <cstdio>
    class Driver;

    extern std::FILE* yyin;
}

%param { Driver& driver }

%locations
%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
    #include <iostream>
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
    ;

%token A C D CD F

%token <UnqualifiedIdentifier> UID "raw_unqualified_id"
%token <std::string> IDENTIFIER "identifier" STRING "string";
%token <std::int64_t> INTEGER "integer";

%%

%start target;

target
    : statements
    ;

statements
    : statements complete_statement
    | complete_statement
    ;

complete_statement
    : statement {driver.push($1);}
    | ENDLINE
    | "once" ENDLINE {driver.parsed->once = true;}
    | error ENDLINE
    ;

%nterm <Statement*> statement;
statement
    : label {$$ = $1;}
    | statement_endline ENDLINE {$$ = $1;}
    ;

%nterm <Statement*> statement_endline;
statement_endline
    : instruction {$$ = $1;}
    | symbol {$$ = $1;}
    | "section" STRING {$$ = new SectionStatement{@$, $2};}
    | "address" expression {$$ = new AddressStatement{@$, $2};}
    | "align" expression {$$ = new AlignStatement{@$, $2};}
    | "res" expression {$$ = new ReserveStatement{@$, $2};}
    | "data" data_element_list {$$ = new DataStatement(@$, $2, 1);}
    | "dataw" data_element_list {$$ = new DataStatement(@$, $2, 2);}
    | "include" STRING {$$ = new IncludeStatement(@$, IncludeStatement::Type::Assembly, $2);}
    | "include_bin" STRING {$$ = new IncludeStatement(@$, IncludeStatement::Type::Binary, $2);}
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

%nterm <std::vector<Expression*>> expression_list;
expression_list
    : expression {$$ = {$1};}
    | expression_list "," expression {$$ = $1; $$.push_back($3);}
    ;

%nterm <InstructionStatement*> instruction;
instruction
    : IDENTIFIER {$$ = new InstructionStatement{@$, {$1, {Mode::Implied}}};}
    | IDENTIFIER mode {$$ = new InstructionStatement{@$, {$1, $2.first}, $2.second};}
    | IDENTIFIER mode "," mode {$$ = new InstructionStatement{@$, {$1, {$2.first, $4.first}}, $2.second, $4.second};}
    ;

%nterm <std::pair<Address, Expression*>> mode;
mode
    : reg         {$$ = {$1, nullptr};}
    | expression  {$$ = {Mode::Immediate, $1};}
    | "[" reg "]" {$$ = {{Mode::Indirect, $2}, nullptr};}
    | "[" expression "]" {$$ = {Mode::Direct, $2};}
    | "[" reg "," expression "]" {$$ = {{Mode::Offset, $2}, $4};}
    ;

%nterm <RID> reg;
reg
    : A  {$$ = RID::A;}
    | C  {$$ = RID::C;}
    | D  {$$ = RID::D;}
    | CD {$$ = RID::CD;}
    | F  {$$ = RID::F;}
    ;

%nterm <LabelStatement*> label;
label
    : ident ":" {$$ = new LabelStatement{@$, $1};}
    ;

%nterm <SymbolStatement*> symbol;
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

