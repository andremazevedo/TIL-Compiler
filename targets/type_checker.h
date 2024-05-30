#ifndef __TIL_TARGETS_TYPE_CHECKER_H__
#define __TIL_TARGETS_TYPE_CHECKER_H__

#include "targets/basic_ast_visitor.h"

#include <stack>

namespace til {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public basic_ast_visitor {
    cdk::symbol_table<til::symbol> &_symtab;
    std::stack<std::shared_ptr<til::symbol>> _functions;
    basic_ast_visitor *_parent;

    std::shared_ptr<cdk::basic_type> _inBlockReturnType = nullptr;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<til::symbol> &symtab, 
                 std::stack<std::shared_ptr<til::symbol>> &functions, basic_ast_visitor *parent) :
        basic_ast_visitor(compiler), _symtab(symtab), _functions(functions), _parent(parent) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    void do_UnaryExpression(cdk::unary_operation_node *const node, int lvl);
    void processBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    void do_IntExpression(cdk::binary_operation_node *const node, int lvl);
    void do_IntDoubleExpression(cdk::binary_operation_node *const node, int lvl);
    void do_IntDoublePointerExpression(cdk::binary_operation_node *const node, int lvl, bool sub);
    template<typename T>
    void process_literal(cdk::literal_node<T> *const node, int lvl) {
    }
  
  protected:
    void do_BooleanLogicalExpression(cdk::binary_operation_node * const node, int lvl);

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end

  };

} // til

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, functions, node) { \
  try { \
    til::type_checker checker(compiler, symtab, functions, this); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, _functions, node)

#endif
