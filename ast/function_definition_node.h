#ifndef __TIL_AST_FUNCTION_DEFINITION_NODE_H__
#define __TIL_AST_FUNCTION_DEFINITION_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "block_node.h"

namespace til {

  /**
   * Class for describing funtion definition nodes.
   */
  class function_definition_node : public cdk::expression_node {
    cdk::sequence_node *_arguments;
    til::block_node *_block;

  public:
    function_definition_node(int lineno, std::shared_ptr<cdk::basic_type> funType, cdk::sequence_node *arguments, 
                             til::block_node *block) :
        cdk::expression_node(lineno), _arguments(arguments), _block(block) {
      type(funType);
    }

    cdk::sequence_node *arguments() { return _arguments; }

    til::block_node *block() { return _block; }

    void accept(basic_ast_visitor *sp, int level) { sp->do_function_definition_node(this, level); }

  };

} // til

#endif
