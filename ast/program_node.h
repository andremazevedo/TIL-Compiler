#ifndef __TIL_AST_PROGRAM_NODE_H__
#define __TIL_AST_PROGRAM_NODE_H__

#include <cdk/ast/expression_node.h>
#include "block_node.h"

namespace til {

  /**
   * Class for describing program nodes.
   */
  class program_node : public cdk::expression_node {
    til::block_node *_block;

  public:
    program_node(int lineno, std::shared_ptr<cdk::basic_type> proType, til::block_node *block) :
        cdk::expression_node(lineno), _block(block) {
      type(proType);
    }

    til::block_node *block() { return _block; }

    void accept(basic_ast_visitor *sp, int level) { sp->do_program_node(this, level); }

  };

} // til

#endif
