#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#include "til_parser.tab.h"

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void til::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void til::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void til::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void til::type_checker::do_UnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else {
    throw std::string("wrong type in unary expression");
  }

  // in Simple, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  do_UnaryExpression(node, lvl);
}

void til::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  do_UnaryExpression(node, lvl);
}

void til::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else {
    throw std::string("wrong type in unary logical expression");
  }
}

//---------------------------------------------------------------------------

void til::type_checker::do_IntExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) 
    throw std::string("integer expression expected in binary operator (left)");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) 
    throw std::string("integer expression expected in binary operator (right)");

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_IntDoubleExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    // TODO: check if this is correct
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else {
    throw std::string("wrong types in binary expression");
  }
}

void til::type_checker::do_IntDoublePointerExpression(cdk::binary_operation_node *const node, int lvl, bool sub = false) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (sub && node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    // TODO: check if pointers are compatible
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->right()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    // TODO: check if this is correct
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else {
    throw std::string("wrong types in binary expression");
  }
}

void til::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  // TODO: change this function
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in right argument of binary expression");

  // in Simple, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  do_IntDoublePointerExpression(node, lvl);
}
void til::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  do_IntDoublePointerExpression(node, lvl, true);
}
void til::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  do_IntDoubleExpression(node, lvl);
}
void til::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  do_IntDoubleExpression(node, lvl);
}
void til::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  do_IntExpression(node, lvl);
}
void til::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void til::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void til::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void til::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void til::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void til::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

//protected:
void til::type_checker::do_BooleanLogicalExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in binary expression");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in binary expression");

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void til::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  do_BooleanLogicalExpression(node, lvl);
}
void til::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  do_BooleanLogicalExpression(node, lvl);
}

//---------------------------------------------------------------------------

void til::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (!symbol)
    throw std::string("undeclared variable '" + id + "'");
  node->type(symbol->type());
}

void til::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  node->type(node->lvalue()->type());
}

void til::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else {
      throw std::string("wrong assignment to integer");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else {
      throw std::string("wrong assignment to real");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {
    if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else {
      throw std::string("wrong assignment to string");
    }
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      throw std::string("wrong assignment to pointer");
    }

    auto rvalue_type = cdk::reference_type::cast(node->rvalue()->type());

    if (rvalue_type->referenced()->name() == cdk::TYPE_UNSPEC) {
      node->rvalue()->type(node->lvalue()->type());
    }

    // TODO: check if pointers are compatible

    node->type(node->lvalue()->type());
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
    if (!node->rvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
      throw std::string("wrong assignment to function");
    }

    // todo: go deeper into cheking the types that compare TYPE_FUNCTIONAL/TYPE_FUNCTIONAL(see if input/output match)

    node->type(node->lvalue()->type());
  }
  else {
    throw std::string("wrong types in assignment");
  }
}

//---------------------------------------------------------------------------

void til::type_checker::do_block_node(til::block_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_program_node(til::program_node *const node, int lvl) {
  auto symbol = til::make_symbol(node->type(), "_main", tPUBLIC);
  _symtab.insert("_main", symbol);
  _parent->set_new_symbol(symbol); // advise parent that a symbol has been inserted
}

void til::type_checker::do_evaluation_node(til::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void til::type_checker::do_print_node(til::print_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_read_node(til::read_node *const node, int lvl) {
  // try {
  //   node->argument()->accept(this, lvl);
  // } catch (const std::string &id) {
  //   throw "undeclared variable '" + id + "'";
  // }
}

//---------------------------------------------------------------------------

void til::type_checker::do_loop_node(til::loop_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void til::type_checker::do_stop_node(til::stop_node *const node, int lvl) {
  // EMPTY
}

void til::type_checker::do_next_node(til::next_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void til::type_checker::do_if_node(til::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT))
    throw std::string("expected integer condition");
}

void til::type_checker::do_if_else_node(til::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_INT))
    throw std::string("expected integer condition");
}

//---------------------------------------------------------------------------

void til::type_checker::do_function_definition_node(til::function_definition_node *const node, int lvl) {
  // EMPTY
}

void til::type_checker::do_function_call_node(til::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);

  if (!node->expression()->is_typed(cdk::TYPE_FUNCTIONAL))
    throw std::string("expression cannot be used as a function");

  auto expression_type = cdk::functional_type::cast(node->expression()->type());

  node->type(expression_type->output(0));

  if (node->arguments()) {
    if (expression_type->input_length() == node->arguments()->size()) {
      for (size_t i = 0; i < node->arguments()->size(); i++) {
        auto argument = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
        argument->accept(this, lvl + 2);
        
        if (expression_type->input(i) == argument->type()) {
          if (expression_type->input(i)->name() == cdk::TYPE_FUNCTIONAL) {
            // TODO: go deeper into cheking the types that compare TYPE_FUNCTIONAL/TYPE_FUNCTIONAL(see if input/output match)
          }
          continue;
        }
        else if (expression_type->input(i)->name() == cdk::TYPE_DOUBLE && argument->is_typed(cdk::TYPE_INT)) {
          continue;
        }
        else {
          throw std::string("wrong type for input");
        }
      }
    }
    else {
      throw std::string("wrong length for input");
    }
  }
  else if (expression_type->input_length()) {
    throw std::string("wrong length for input");
  }
}

//---------------------------------------------------------------------------

void til::type_checker::do_return_node(til::return_node *const node, int lvl) {
  auto function_type = cdk::functional_type::cast(_functions.top()->type());

  if (node->retval()) {
    if (function_type->output(0)->name() == cdk::TYPE_VOID)
      throw std::string("return value specified for void function.");

    node->retval()->accept(this, lvl + 2);

    if (function_type->output(0)->name() == cdk::TYPE_INT) {
      if (!node->retval()->is_typed(cdk::TYPE_INT))
        throw std::string("wrong type for return value (integer expected).");
    }
    else if (function_type->output(0)->name() == cdk::TYPE_DOUBLE) {
      if (!(node->retval()->is_typed(cdk::TYPE_INT) || node->retval()->is_typed(cdk::TYPE_DOUBLE)))
        throw std::string("wrong type for return value (integer or double expected).");
    }
    else if (function_type->output(0)->name() == cdk::TYPE_STRING) {
      if (!node->retval()->is_typed(cdk::TYPE_STRING))
        throw std::string("wrong type for return value (string expected).");
    }
    else if (function_type->output(0)->name() == cdk::TYPE_POINTER) {
      if (!node->retval()->is_typed(cdk::TYPE_POINTER))
        throw std::string("wrong type for return value (pointer expected).");

      auto retval_type = cdk::reference_type::cast(node->retval()->type());
      // auto function_output_type = cdk::reference_type::cast(function_type->output(0));
      
      if (retval_type->referenced()->name() == cdk::TYPE_UNSPEC)
        node->retval()->type(function_type->output(0));

      // TODO: check if pointers are compatible
    }
    else {
      throw std::string("unknown type for return value.");
    }
  }
  else if (function_type->output(0)->name() != cdk::TYPE_VOID) {
    throw std::string("no return value for non-void function.");
  }
}

//---------------------------------------------------------------------------

void til::type_checker::do_variable_declaration_node(til::variable_declaration_node *const node, int lvl) {
  if (node->qualifier() == tEXTERNAL) {
    if (!node->is_typed(cdk::TYPE_FUNCTIONAL))
      throw std::string("wrong type for external variable (functional type expected).");
  }
  
  if (node->initializer()) {
    node->initializer()->accept(this, lvl + 2);

    if (!node->type()) {
      // vars are typed by their initializers
      if (node->initializer()->is_typed(cdk::TYPE_UNSPEC))
        throw std::string("unknown type for initializer.");
      node->type(node->initializer()->type());
      // node->type(cdk::primitive_type::create(node->initializer()->type()->size(), node->initializer()->type()->name()));
    }
    else if (node->is_typed(cdk::TYPE_INT)) {
      if (!node->initializer()->is_typed(cdk::TYPE_INT))
        throw std::string("wrong type for initializer (integer expected).");
    }
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (!(node->initializer()->is_typed(cdk::TYPE_INT) || node->initializer()->is_typed(cdk::TYPE_DOUBLE)))
        throw std::string("wrong type for initializer (integer or double expected).");
    }
    else if (node->is_typed(cdk::TYPE_STRING)) {
      if (!node->initializer()->is_typed(cdk::TYPE_STRING))
        throw std::string("wrong type for initializer (string expected).");
    }
    else if (node->is_typed(cdk::TYPE_POINTER)) {
      if (!node->initializer()->is_typed(cdk::TYPE_POINTER))
        throw std::string("wrong type for initializer (pointer expected).");

      auto initializer_type = cdk::reference_type::cast(node->initializer()->type());
      
      if (initializer_type->referenced()->name() == cdk::TYPE_UNSPEC) {
        node->initializer()->type(node->type());
      }
    }
    else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
      if (!node->initializer()->is_typed(cdk::TYPE_FUNCTIONAL))
        throw std::string("wrong type for initializer (function expected).");

      auto initializer_type = cdk::functional_type::cast(node->initializer()->type());
      auto node_type = cdk::functional_type::cast(node->type());

      // TODO: go deeper into cheking the types that compare TYPE_FUNCTIONAL/TYPE_FUNCTIONAL(see if input/output match)
      // TODO: RECUSIVE CHECKING
      // TODO: check for doubles as well 
      if (node_type->output(0) == initializer_type->output(0)) {
        if (node_type->output(0)->name() == cdk::TYPE_FUNCTIONAL) {
          // TODO: go deeper into cheking the types that compare TYPE_FUNCTIONAL/TYPE_FUNCTIONAL(see if input/output match)
        }
      }
      else {
        throw std::string("wrong type for initializer output.");
      }
      
      // arguments/inputs
      if (node_type->input_length() == initializer_type->input_length()) {
        for (size_t i = 0; i < initializer_type->input_length(); i++) {
          if (node_type->input(i) == initializer_type->input(i)) {
            if (node_type->input(i)->name() == cdk::TYPE_FUNCTIONAL) {
              // TODO: go deeper into cheking the types that compare TYPE_FUNCTIONAL/TYPE_FUNCTIONAL(see if input/output match)
            }
          }
          else {
            throw std::string("wrong type for initializer input.");
          }
        }
      }
      else {
        throw std::string("wrong length for initializer input.");
      }

    }
    else {
      throw std::string("unknown type for initializer.");
    }
  }

  const std::string &id = node->identifier();
  auto symbol = til::make_symbol(node->type(), id, node->qualifier());

  auto previous = _symtab.find_local(id);
  if (previous) {
    if (previous->qualifier() == tFORWARD) {
      // TODO: check compatibility of types
      symbol->qualifier(tPUBLIC);
      _symtab.replace(id, symbol);
      _parent->set_new_symbol(symbol); // advise parent that a symbol has been inserted
    }
    else {
      throw std::string("variable '" + id + "' redeclared");
    }
  }
  else {
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol); // advise parent that a symbol has been inserted
  }
  
}

//---------------------------------------------------------------------------

void til::type_checker::do_nullptr_node(til::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC)));
}

void til::type_checker::do_index_node(til::index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->base()->accept(this, lvl + 2);

  if (!node->base()->is_typed(cdk::TYPE_POINTER))
    throw std::string("pointer expression expected in index left-value");

  auto base_type = cdk::reference_type::cast(node->base()->type());

  if (base_type->referenced()->name() == cdk::TYPE_FUNCTIONAL)
    throw std::string("Cannot index function pointers.");

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in left-value index");

  node->type(base_type->referenced());
}

void til::type_checker::do_stack_alloc_node(til::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT))
    throw std::string("integer expression expected in allocation expression");

  node->type(cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC)));
}

void til::type_checker::do_address_of_node(til::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

//---------------------------------------------------------------------------

void til::type_checker::do_sizeof_node(til::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}
