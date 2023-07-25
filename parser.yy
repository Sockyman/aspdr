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
    #include <SpdrFirmware/Register.hpp>
    #include <SpdrFirmware/Mode.hpp>
    #include <string>
    #include <cstdint>
    #include <memory>
    #include <tuple>
    #include <vector>
    class Driver;
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

%token IMPORT "import"
    IMPORT_ONCE "import_once"
    IMPORT_BIN "import_bin"
    ADDRESS "address"
    ALIGN "align"
    RES "res"
    RESW "resw"
    BYTE "byte"
    WORD "word"
    ;

%token A C D CD F

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
    ;

%nterm <Statement*> statement;
statement
    : instruction ENDLINE {$$ = $1;}
    | symbol ENDLINE {$$ = $1;}
    | label {$$ = $1;}
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
    : ident_tail ":" {$$ = new LabelStatement{@$, $1};}
    ;

%nterm <SymbolStatement*> symbol;
symbol
    : ident_tail "=" expression {$$ = new SymbolStatement{@$, $1, $3};}
    ;

%nterm <Identifier> ident;
ident
    : "." ident {$$ = Identifier();}
    | ident_tail {$$ = Identifier();}
    ;

%nterm <Identifier> ident_tail;
ident_tail
    : IDENTIFIER {$$ = Identifier{$1};}
    | ident_tail "." IDENTIFIER {$$ = $1; $1.push($3);}
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
    | ident_tail {$$ = new SymbolicExpression{@$, $1};}
    ;

%%

void yy::parser::error(const location_type& loc, const std::string& message) {
    std::cerr <<  loc << " \033[31merror:\033[0m " << message << '\n';
}

