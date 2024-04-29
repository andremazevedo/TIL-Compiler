#ifndef __TIL_AST_FUNCTION_DEFINITION_NODE_H__
#define __TIL_AST_FUNCTION_DEFINITION_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace til {

  /**
   * Class for describing funtion definition nodes.
   */
  class function_definition_node : public cdk::expression_node {
    cdk::sequence_node *_arguments;
    cdk::sequence_node *_declarations, *_instructions;

  public:
    function_definition_node(int lineno, std::shared_ptr<cdk::basic_type> funType, cdk::sequence_node *arguments, 
                             cdk::sequence_node *declarations, cdk::sequence_node *instructions) :
        cdk::expression_node(lineno), _arguments(arguments), _declarations(declarations), _instructions(instructions) {
      type(funType);
    }

    cdk::sequence_node *arguments() { return _arguments; }

    cdk::sequence_node *declarations() { return _declarations; }

    cdk::sequence_node *instructions() { return _instructions; }

    void accept(basic_ast_visitor *sp, int level) { sp->do_function_definition_node(this, level); }

  };

} // til

#endif
