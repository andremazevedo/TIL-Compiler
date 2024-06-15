#ifndef __TIL_TARGETS_POSTFIX_WRITER_H__
#define __TIL_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <set>
#include <vector>
#include <stack>
#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>

namespace til {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<til::symbol> &_symtab;

    std::set<std::string> _functions_to_declare;

    // semantic analysis
    int _inFunctionArgs, _inFunctionBody;
    std::vector<int> _loopTest, _loopEnd; // for stop/next
    std::stack<std::shared_ptr<til::symbol>> _functions; // for keeping track of the current functions
    std::shared_ptr<til::symbol> _function_symbol; // last function symbol found in symbol table or defined
    int _offset; // current framepointer offset (0 means no vars defined)

    std::stack<int> _bodyRetLabel; // where to jump when a return occurs

    cdk::basic_postfix_emitter &_pf;
    int _lbl;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<til::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _inFunctionArgs(0), _inFunctionBody(0), _offset(0), 
        _pf(pf), _lbl(0) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  public:
    std::shared_ptr<til::symbol> function_symbol() {
      return _function_symbol;
    }

    void set_function_symbol(std::shared_ptr<til::symbol> symbol) {
      _function_symbol = symbol;
    }

    void reset_function_symbol() {
      _function_symbol = nullptr;
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // til

#endif
