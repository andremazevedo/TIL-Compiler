%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;          /* integer value */
  double                d;          /* double value */
  std::string          *s;          /* symbol name or string literal */
  cdk::basic_node      *basic;      /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;

  til::block_node      *block;
  std::vector<std::shared_ptr<cdk::basic_type>> *types;
};

%token tTYPE_INT tTYPE_DOUBLE tTYPE_STRING tTYPE_VOID
%token tPRIVATE tEXTERNAL tFORWARD tPUBLIC tVAR
%token tBLOCK tIF tLOOP tSTOP tNEXT tRETURN tPRINT tPRINTLN
%token tREAD tNULL tSET tINDEX tOBJECTS tSIZEOF tFUNCTION
%token tPROGRAM
%token tLE tGE tEQ tNE tAND tOR

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING

%type <basic> declaration argument_declaration program instruction
%type <expression> expression function
%type <sequence> file declarations expressions argument_declarations instructions
%type <lvalue> lvalue

%type <block> block
%type <type> type data_type function_type void_type
%type <types> types

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}

%%

file : /* empty */          { compiler->ast($$ = new cdk::sequence_node(LINE)); }
     | declarations         { compiler->ast($$ = $1); }
     | program              { compiler->ast($$ = new cdk::sequence_node(LINE, $1)); }
     | declarations program { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1)); }
     ;

declaration : '(' type tIDENTIFIER ')'                    { $$ = new til::variable_declaration_node(LINE, tPRIVATE, $2, *$3, nullptr); delete $3; }
            | '(' type tIDENTIFIER expression ')'         { $$ = new til::variable_declaration_node(LINE, tPRIVATE, $2, *$3, $4); delete $3; }
            | '(' tPUBLIC type tIDENTIFIER ')'            { $$ = new til::variable_declaration_node(LINE, tPUBLIC, $3, *$4, nullptr); delete $4; }
            | '(' tPUBLIC type tIDENTIFIER expression ')' { $$ = new til::variable_declaration_node(LINE, tPUBLIC, $3, *$4, $5); delete $4; }
            | '(' tFORWARD type tIDENTIFIER ')'           { $$ = new til::variable_declaration_node(LINE, tFORWARD, $3, *$4, nullptr); delete $4; }
            | '(' tEXTERNAL type tIDENTIFIER ')'          { $$ = new til::variable_declaration_node(LINE, tEXTERNAL, $3, *$4, nullptr); delete $4; }
            /* var */
            | '(' tVAR tIDENTIFIER expression ')'         { $$ = new til::variable_declaration_node(LINE, tPRIVATE, nullptr, *$3, $4); delete $3; }
            | '(' tPUBLIC tVAR tIDENTIFIER expression ')' { $$ = new til::variable_declaration_node(LINE, tPUBLIC, nullptr, *$4, $5); delete $4; }
            | '(' tPUBLIC tIDENTIFIER expression ')'      { $$ = new til::variable_declaration_node(LINE, tPUBLIC, nullptr, *$3, $4); delete $3; }
            ;

declarations : declaration              { $$ = new cdk::sequence_node(LINE, $1); }
             | declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

argument_declaration : '(' type tIDENTIFIER ')' { $$ = new til::variable_declaration_node(LINE, tPRIVATE, $2, *$3, nullptr); delete $3; }
                     ;

argument_declarations : argument_declaration                       { $$ = new cdk::sequence_node(LINE, $1); }
                      | argument_declarations argument_declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
                      ;

program : '(' tPROGRAM block ')' { $$ = new til::program_node(LINE, $3); }
        ;

function : '(' tFUNCTION '(' type ')' block ')'                       { $$ = new til::function_definition_node(LINE, $4, nullptr, $6); }
         | '(' tFUNCTION '(' type argument_declarations ')' block ')' { $$ = new til::function_definition_node(LINE, $4, $5, $7); }
         ;

type : data_type     { $$ = $1; }
     | function_type { $$ = $1; }
     | void_type     { $$ = $1; }
     ;

types : type       { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }
      | types type { $$ = $1; $$->push_back($2); }
      ;

data_type : tTYPE_INT     { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
          | tTYPE_DOUBLE  { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
          | tTYPE_STRING  { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
          | data_type '!' { $$ = cdk::reference_type::create(4, $1); }
          ;

function_type : '(' type ')'               { $$ = cdk::functional_type::create($2); }
              | '(' type '(' types ')' ')' { $$ = cdk::functional_type::create(*$4, $2); delete $4; }
              | function_type '!'          { $$ = cdk::reference_type::create(4, $1); }
              ;

void_type : tTYPE_VOID    { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
          | void_type '!' { $$ = cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_VOID)); }
          ;

block : /* empty */               { $$ = new til::block_node(LINE, nullptr, nullptr); }
      | declarations              { $$ = new til::block_node(LINE, $1, nullptr); }
      | instructions              { $$ = new til::block_node(LINE, nullptr, $1); }
      | declarations instructions { $$ = new til::block_node(LINE, $1, $2); }
      ;

instruction : '(' tBLOCK block ')'                           { $$ = $3; }
            | '(' tIF expression instruction ')'             { $$ = new til::if_node(LINE, $3, $4); }
            | '(' tIF expression instruction instruction ')' { $$ = new til::if_else_node(LINE, $3, $4, $5); }
            | '(' tLOOP expression instruction ')'           { $$ = new til::loop_node(LINE, $3, $4); }
            | '(' tSTOP ')'                                  { $$ = new til::stop_node(LINE); }
            | '(' tSTOP tINTEGER ')'                         { $$ = new til::stop_node(LINE, $3); }
            | '(' tNEXT ')'                                  { $$ = new til::next_node(LINE); }
            | '(' tNEXT tINTEGER ')'                         { $$ = new til::next_node(LINE, $3); }
            | '(' tRETURN ')'                                { $$ = new til::return_node(LINE); }
            | '(' tRETURN expression ')'                     { $$ = new til::return_node(LINE, $3); }
            | expression                                     { $$ = new til::evaluation_node(LINE, $1); }
            | '(' tPRINT expressions ')'                     { $$ = new til::print_node(LINE, $3); }
            | '(' tPRINTLN expressions ')'                   { $$ = new til::print_node(LINE, $3, true); }
            ;

instructions : instruction              { $$ = new cdk::sequence_node(LINE, $1); }
             | instructions instruction { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

expression : tINTEGER                           { $$ = new cdk::integer_node(LINE, $1); }
           | tDOUBLE                            { $$ = new cdk::double_node(LINE, $1); }
           | tSTRING                            { $$ = new cdk::string_node(LINE, $1); delete $1; }
           | tNULL                              { $$ = new til::nullptr_node(LINE); }
           /* unary expressions */
           | '(' '-' expression ')'             { $$ = new cdk::unary_minus_node(LINE, $3); }
           | '(' '+' expression ')'             { $$ = new cdk::unary_plus_node(LINE, $3); }
           /* arithmetic expressions */
           | '(' '+' expression expression ')'  { $$ = new cdk::add_node(LINE, $3, $4); }
           | '(' '-' expression expression ')'  { $$ = new cdk::sub_node(LINE, $3, $4); }
           | '(' '*' expression expression ')'  { $$ = new cdk::mul_node(LINE, $3, $4); }
           | '(' '/' expression expression ')'  { $$ = new cdk::div_node(LINE, $3, $4); }
           | '(' '%' expression expression ')'  { $$ = new cdk::mod_node(LINE, $3, $4); }
           /* logical expressions */
           | '(' '<' expression expression ')'  { $$ = new cdk::lt_node(LINE, $3, $4); }
           | '(' '>' expression expression ')'  { $$ = new cdk::gt_node(LINE, $3, $4); }
           | '(' tLE expression expression ')'  { $$ = new cdk::le_node(LINE, $3, $4); }
           | '(' tGE expression expression ')'  { $$ = new cdk::ge_node(LINE, $3, $4); }
           | '(' tEQ expression expression ')'  { $$ = new cdk::eq_node(LINE, $3, $4); }
           | '(' tNE expression expression ')'  { $$ = new cdk::ne_node(LINE, $3, $4); }
           | '(' '~' expression ')'             { $$ = new cdk::not_node(LINE, $3); }
           | '(' tAND expression expression ')' { $$ = new cdk::and_node(LINE, $3, $4); }
           | '(' tOR expression expression ')'  { $$ = new cdk::or_node (LINE, $3, $4); }
           /* assignemnts */
           | '(' tSET lvalue expression ')'     { $$ = new cdk::assignment_node(LINE, $3, $4); }
           /* identifiers */
           | lvalue                             { $$ = new cdk::rvalue_node(LINE, $1); }
           /* read */
           | '(' tREAD ')'                      { $$ = new til::read_node(LINE); }
           /* functions */
           | function                           { $$ = $1; }
           | '(' expression ')'                 { $$ = new til::function_call_node(LINE, $2, nullptr); }
           | '(' expression expressions ')'     { $$ = new til::function_call_node(LINE, $2, $3); }
           | '(' '@' ')'                        { $$ = new til::function_call_node(LINE, nullptr, nullptr); }
           | '(' '@' expressions ')'            { $$ = new til::function_call_node(LINE, nullptr, $3); }
           /* others */
           | '(' tOBJECTS expression ')'        { $$ = new til::stack_alloc_node(LINE, $3); }
           | '(' '?' lvalue ')'                 { $$ = new til::address_of_node(LINE, $3); }
           | '(' tSIZEOF expression ')'         { $$ = new til::sizeof_node(LINE, $3); }
           ;

expressions : expression             { $$ = new cdk::sequence_node(LINE, $1); }
            | expressions expression { $$ = new cdk::sequence_node(LINE, $2, $1); }
            ;

lvalue : tIDENTIFIER                          { $$ = new cdk::variable_node(LINE, $1); delete $1; }
       | '(' tINDEX expression expression ')' { $$ = new til::index_node(LINE, $3, $4); }
       ;

%%
