%option c++ prefix="til_scanner_" outfile="til_scanner.cpp"
%option stack noyywrap yylineno 8bit debug

%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "til_parser.tab.h"

// don't change this
#define yyerror LexerError
%}

%x X_COMMENT X_STRING X_NULL X_BACKSLASH

%%
  yydebug=1; set_debug(1);

  /* comments */
"//".*$                 ;

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ;

  /* types */
"int"                  return tTYPE_INT;
"double"               return tTYPE_DOUBLE;
"string"               return tTYPE_STRING;
"void"                 return tTYPE_VOID;

  /* statements */
"external"             return tEXTERNAL;
"forward"              return tFORWARD;
"public"               return tPUBLIC;
"var"                  return tVAR;

  /* instructions */
"block"                return tBLOCK;
"if"                   return tIF;
"loop"                 return tLOOP;
"stop"                 return tSTOP;
"next"                 return tNEXT;
"return"               return tRETURN;
"print"                return tPRINT;
"println"              return tPRINTLN;

  /* expressions */
"read"                 return tREAD;
"null"                 return tNULL;
"set"                  return tSET;
"index"                return tINDEX;
"objects"              return tOBJECTS;
"sizeof"               return tSIZEOF;
"function"             return tFUNCTION;

  /* others */
"program"              return tPROGRAM;

  /* expressions operators */
[-+?*/%<>~]            return *yytext;
"<="                   return tLE;
">="                   return tGE;
"=="                   return tEQ;
"!="                   return tNE;
"&&"                   return tAND;
"||"                   return tOR;

  /* delimiters */
[()]                   return *yytext;

  /* identifiers */
[A-Za-z][A-Za-z0-9]*   yylval.s = new std::string(yytext); return tIDENTIFIER;

  /* literals integers*/
  /* TODO */
[0-9]+                 yylval.i = strtol(yytext, nullptr, 10); return tINTEGER;

  /* literals doubles */
  /* TODO */

  /* literals strings*/
\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
  /* <X_STRING>\\0           yy_push_state(X_NULL); */
  /* <X_STRING>\\           yy_push_state(X_BACKSLASH); */
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>\\\"         *yylval.s += yytext + 1;
<X_STRING>\\\\         *yylval.s += yytext + 1;
<X_STRING>.            *yylval.s += yytext;
<X_STRING>\n           yyerror("newline in string");

[-()<>=+*/%;{}.]       return *yytext;

  /* whitespaces */
[\n\r \t]+             ;

.                      yyerror("Unknown character");

%%
