
%option noyywrap nounput noinput batch

%{

#include <string>
#include "Driver.hpp"
#include "parser.hpp"

#define YY_USER_ACTION {loc.columns(yyleng);}

%}


%%

%{
    yy::location& loc = driver.location;
    loc.step();
%}

"import" { return yy::parser::make_IMPORT(loc); }
"import_once" { return yy::parser::make_IMPORT_ONCE(loc); }
"import_bin" { return yy::parser::make_IMPORT_BIN(loc); }
"address" { return yy::parser::make_ADDRESS(loc); }
"align" { return yy::parser::make_ALIGN(loc); }
"res" { return yy::parser::make_RES(loc); }
"resw" { return yy::parser::make_RESW(loc); }
"byte" { return yy::parser::make_BYTE(loc); }
"word" { return yy::parser::make_WORD(loc); }

"a" { return yy::parser::make_A(loc); }
"c" { return yy::parser::make_C(loc); }
"d" { return yy::parser::make_D(loc); }
"cd" { return yy::parser::make_CD(loc); }
"f" { return yy::parser::make_F(loc); }

[a-zA-Z_][a-zA-Z_0-9]*  { return yy::parser::make_IDENTIFIER(yytext, loc); }
[0-9]+                  { return yy::parser::make_INTEGER(std::stoi(yytext), loc); }
0[xX][0-9a-fA-F]+       { return yy::parser::make_INTEGER(std::stoi(yytext), loc); }


\n  {
    loc.lines(yyleng);
    loc.step();
    return yy::parser::make_ENDLINE(loc);
}

\;.* { loc.step(); }

<<EOF>>  {
    if (driver.reachedEof) {
        return yy::parser::make_YYEOF(loc);
    } else {
        driver.reachedEof = true;
        return yy::parser::make_ENDLINE(loc);
    }
}


[ \t\r] { loc.step(); }

">=" { return yy::parser::make_GREATER_EQUAL(loc); }
"<=" { return yy::parser::make_LESS_EQUAL(loc); }
"==" { return yy::parser::make_EQUAL(loc); }
"!=" { return yy::parser::make_NOT_EQUAL(loc); }
"&&" { return yy::parser::make_LOG_AND(loc); }
"||" { return yy::parser::make_LOG_OR(loc); }

"<<" { return yy::parser::make_LEFT_SHIFT(loc); }
">>" { return yy::parser::make_RIGHT_SHIFT(loc); }

"," { return yy::parser::make_COMMA(loc); }
":" { return yy::parser::make_COLON(loc); }
"(" { return yy::parser::make_LPAREN(loc); }
")" { return yy::parser::make_RPAREN(loc); }
"[" { return yy::parser::make_LBRACKET(loc); }
"]" { return yy::parser::make_RBRACKET(loc); }
"+" { return yy::parser::make_PLUS(loc); }
"-" { return yy::parser::make_MINUS(loc); }
"*" { return yy::parser::make_ASTRIX(loc); }
"/" { return yy::parser::make_SLASH(loc); }
"%" { return yy::parser::make_PERCENT(loc); }
"&" { return yy::parser::make_AND(loc); }
"|" { return yy::parser::make_OR(loc); }
"^" { return yy::parser::make_XOR(loc); }
"!" { return yy::parser::make_NOT(loc); }
"~" { return yy::parser::make_INVERT(loc); }
"=" { return yy::parser::make_ASSIGN(loc); }
"<" { return yy::parser::make_LESS(loc); }
">" { return yy::parser::make_GREATER(loc); }
"." { return yy::parser::make_DOT(loc); }



. { throw yy::parser::syntax_error(loc, "syntax error, unexpected \'" + std::string(yytext) + "\'"); }

%%

