%option c++ prefix="til_scanner_" outfile="til_scanner.cpp"
%option stack noyywrap yylineno 8bit debug

%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "til_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

// don't change this
#define yyerror LexerError

int stringToInteger(const std::string &str, int base) {
  try {
    return std::stoi(str, nullptr, base);
  }
  catch (const std::out_of_range& e) {
    // TODO std::cerr << "error: line " << lineno() << ": integer literal overflow" << std::endl; 
    exit(1);
  }
}

double stringToDouble(const std::string &str) {
  try {
    return std::stod(str);
  }
  catch (const std::out_of_range& e) {
    // TODO std::cerr << "error: line " << lineno() << ": double literal overflow" << std::endl; 
    exit(1);
  }
}
%}

%x X_COMMENT
%x X_HEX_INT
%x X_STRING X_BACKSLASH X_NULL

%%
  yydebug=1; set_debug(1);

  /* comments */
;.*$                ;

"/*"                  yy_push_state(X_COMMENT);
<X_COMMENT>"/*"       yy_push_state(X_COMMENT);
<X_COMMENT>"*/"       yy_pop_state();
<X_COMMENT>.|\n       ;

  /* types */
"int"                 return tTYPE_INT;
"double"              return tTYPE_DOUBLE;
"string"              return tTYPE_STRING;
"void"                return tTYPE_VOID;

  /* statements */
"external"            return tEXTERNAL;
"forward"             return tFORWARD;
"public"              return tPUBLIC;
"var"                 return tVAR;

  /* instructions */
"block"               return tBLOCK;
"if"                  return tIF;
"loop"                return tLOOP;
"stop"                return tSTOP;
"next"                return tNEXT;
"return"              return tRETURN;
"print"               return tPRINT;
"println"             return tPRINTLN;

  /* expressions */
"read"                return tREAD;
"null"                return tNULL;
"set"                 return tSET;
"index"               return tINDEX;
"objects"             return tOBJECTS;
"sizeof"              return tSIZEOF;
"function"            return tFUNCTION;

  /* others */
"program"               return tPROGRAM;

  /* expressions operators */
[-+?*/%<>~]           return *yytext;
"<="                  return tLE;
">="                  return tGE;
"=="                  return tEQ;
"!="                  return tNE;
"&&"                  return tAND;
"||"                  return tOR;

  /* delimiters */
[()]                  return *yytext;

  /* identifiers */
[A-Za-z][A-Za-z0-9]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

  /* literals integers*/
[1-9][0-9]*                             yylval.i = stringToInteger(yytext, 10); return tINTEGER;
0                                       yylval.i = stringToInteger(yytext, 10); return tINTEGER;
0[0-9]+                                 std::cerr << "error: line " << lineno() << ": invalid literal" << std::endl; exit(1);

"0x"                                    yy_push_state(X_HEX_INT);
<X_HEX_INT>[0-9A-Fa-f]+                 yylval.i = stringToInteger(yytext, 16); yy_pop_state(); return tINTEGER;
<X_HEX_INT>.|\n                         std::cerr << "error: line " << lineno() << ": invalid hexadecimal literal" << std::endl; exit(1);

  /* literals doubles */
[0-9]*\.[0-9]+([Ee][-+]?[0-9]+)?        yylval.d = stringToDouble(yytext); return tDOUBLE;
[0-9]+\.[0-9]*([Ee][-+]?[0-9]+)?        yylval.d = stringToDouble(yytext); return tDOUBLE;
[0-9]+([Ee][-+]?[0-9]+)                 yylval.d = stringToDouble(yytext); return tDOUBLE;

  /* literals strings*/
\"                            yy_push_state(X_STRING);
<X_STRING>\\                  yy_push_state(X_BACKSLASH);
<X_STRING>\"                  {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                              }
<X_STRING>\0                  yyerror("null in string");
<X_STRING>.                   strlit << *yytext;
<X_STRING>\n                  yyerror("newline in string");

<X_BACKSLASH>0                yy_push_state(X_NULL);
<X_BACKSLASH>t                strlit << '\t'; yy_pop_state();
<X_BACKSLASH>n                strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                strlit << '\r'; yy_pop_state();
<X_BACKSLASH>\"               strlit << '\"'; yy_pop_state();
<X_BACKSLASH>\\               strlit << '\\'; yy_pop_state();
<X_BACKSLASH>[0-7]{1,3}       {
                                strlit << (char)(unsigned char)std::stoi(yytext, nullptr, 8);
                                yy_pop_state();
                              }
<X_BACKSLASH>\0               yyerror("null in string");
<X_BACKSLASH>.                strlit << *yytext; yy_pop_state();
<X_BACKSLASH>\n               yyerror("newline in string");

<X_NULL>\"                    {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                yy_pop_state();
                                yy_pop_state();
                                return tSTRING;
                              }
<X_NULL>\\\"                  ;
<X_NULL>\\\\                  ;
<X_NULL>\0                    yyerror("null in string");
<X_NULL>.                     ;
<X_NULL>\n                    yyerror("newline in string");

[-()<>=+*/%;{}.]      return *yytext;

  /* whitespaces */
[\n\r \t]+            ;

.                     yyerror("Unknown character");

%%
