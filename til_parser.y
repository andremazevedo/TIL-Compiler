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

%nonassoc tIFX
%nonassoc tELSE

%right '='
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY

%type <basic> declaration var_decla fun_decla program stmt
%type <expression> expression function
%type <sequence> file declarations expressions fun_declas list
%type <lvalue> lval

%type <i> qualifier
%type <type> data_type
%type <block> block

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}

%%

file : /* empty */          { compiler->ast($$ = new cdk::sequence_node(LINE)); }
     | declarations         { compiler->ast($$ = $1); }
     | program              { compiler->ast($$ = new cdk::sequence_node(LINE, $1)); }
     | declarations program { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1)); }
     ;

declarations : declaration              { $$ = new cdk::sequence_node(LINE, $1); }
             | declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }

declaration : var_decla { $$ = $1; }
            | fun_decla { $$ = $1; }
            ;

var_decla : '(' qualifier data_type tIDENTIFIER ')'            { $$ = new til::variable_declaration_node(LINE, $2, $3, *$4, nullptr); }
          | '(' data_type tIDENTIFIER expression ')'           { $$ = new til::variable_declaration_node(LINE, tPRIVATE, $2, *$3, $4); }
          | '(' qualifier data_type tIDENTIFIER expression ')' { $$ = new til::variable_declaration_node(LINE, $2, $3, *$4, $5); }

          | '(' tIDENTIFIER expression ')'                     { $$ = new til::variable_declaration_node(LINE, tPRIVATE, nullptr, *$2, $3); }
          | '(' qualifier tIDENTIFIER expression ')'           { $$ = new til::variable_declaration_node(LINE, $2, nullptr, *$3, $4); }
          | '(' tVAR tIDENTIFIER expression ')'                { $$ = new til::variable_declaration_node(LINE, tPRIVATE, nullptr, *$3, $4); }
          | '(' qualifier tVAR tIDENTIFIER expression ')'      { $$ = new til::variable_declaration_node(LINE, $2, nullptr, *$4, $5); }
          ;

fun_decla : '(' data_type tIDENTIFIER ')' { $$ = new til::variable_declaration_node(LINE, tPRIVATE, $2, *$3, nullptr); }
          ;

fun_declas : fun_decla            { $$ = new cdk::sequence_node(LINE, $1); }
           | fun_declas fun_decla { $$ = new cdk::sequence_node(LINE, $2, $1); }
           ;

qualifier : tPUBLIC   { $$ = tPUBLIC; }
          | tFORWARD  { $$ = tFORWARD; }
          | tEXTERNAL { $$ = tEXTERNAL; }
          ;

program : '(' tPROGRAM block ')' { $$ = new til::program_node(LINE, nullptr, $3); }
        ;

function : '(' function '(' data_type ')' block ')'            { $$ = new til::function_definition_node(LINE, $4, nullptr, $6); }
         | '(' function '(' data_type fun_declas ')' block ')' { $$ = new til::function_definition_node(LINE, $4, $5, $7); }
         ;

data_type : tTYPE_INT                        { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
          | tTYPE_DOUBLE                     { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
          | tTYPE_STRING                     { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
          | tTYPE_VOID                       { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
          | data_type '!'                    { $$ = cdk::reference_type::create(4, $1); }
          /* | fun_type                         { $$ = $1; } */
          ;














block : /* empty */                 { $$ = new til::block_node(LINE, nullptr, nullptr); }
      | '(' tBLOCK declarations ')' { $$ = new til::block_node(LINE, $3, nullptr); }
      | '(' tBLOCK list ')'         { $$ = new til::block_node(LINE, nullptr, $3); }
      | declarations                { $$ = new til::block_node(LINE, $1, nullptr); }
      | list                        { $$ = new til::block_node(LINE, nullptr, $1); }
      ;

list : stmt      { $$ = new cdk::sequence_node(LINE, $1); }
     | list stmt { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;

stmt : expression ';'                         { $$ = new til::evaluation_node(LINE, $1); }
     | '(' tPRINT expressions ')'             { $$ = new til::print_node(LINE, $3); }
     | '(' tPRINTLN expressions ')'           { $$ = new til::print_node(LINE, $3, true); }
     | tLOOP '(' expression ')' stmt          { $$ = new til::loop_node(LINE, $3, $5); }
     | tIF '(' expression ')' stmt %prec tIFX { $$ = new til::if_node(LINE, $3, $5); }
     | tIF '(' expression ')' stmt tELSE stmt { $$ = new til::if_else_node(LINE, $3, $5, $7); }
     | '{' list '}'                     { $$ = $2; }
     | '(' tRETURN expression ')'             { $$ = new til::return_node(LINE, $3); }
     ;

expression : tINTEGER                    { $$ = new cdk::integer_node(LINE, $1); }
           | tSTRING                     { $$ = new cdk::string_node(LINE, $1); }
           /* UNARY EXPRESSION */
           | '-' expression %prec tUNARY { $$ = new cdk::unary_minus_node(LINE, $2); }
           | '+' expression %prec tUNARY { $$ = new cdk::unary_plus_node(LINE, $2); }
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
           /* logical expressions */
           | '(' tAND expression expression ')' { $$ = new cdk::and_node(LINE, $3, $4); }
           | '(' tOR expression expression ')'  { $$ = new cdk::or_node (LINE, $3, $4); }
           | '(' expression ')'          { $$ = $2; }
           | lval                        { $$ = new cdk::rvalue_node(LINE, $1); }
           | lval '=' expression         { $$ = new cdk::assignment_node(LINE, $1, $3); }
           | tREAD                       { $$ = new til::read_node(LINE); }
           ;

expressions : expression             { $$ = new cdk::sequence_node(LINE, $1); }
            | expressions expression { $$ = new cdk::sequence_node(LINE, $2, $1); }
            ;

lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
     ;

%%
