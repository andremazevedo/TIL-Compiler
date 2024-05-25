#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include ".auto/all_nodes.h"  // automatically generated

#include "til_parser.tab.h"

//---------------------------------------------------------------------------

void til::postfix_writer::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void til::postfix_writer::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody)
    _pf.INT(node->value()); // integer literal is on the stack: push an integer
  else
    _pf.SINT(node->value()); // integer literal is on the DATA segment
}

void til::postfix_writer::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody)
    _pf.DOUBLE(node->value()); // load number to the stack
  else
    _pf.SDOUBLE(node->value()); // double is on the DATA segment
}

void til::postfix_writer::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  // TODO
  if (_inFunctionBody) {
    /* leave the address on the stack */
    _pf.TEXT(); // return to the TEXT segment
    _pf.ADDR(mklbl(lbl1)); // the string to be printed
  }
  else {
    // global variable initializer
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

void til::postfix_writer::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_add_node(cdk::add_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.ADD();
}
void til::postfix_writer::do_sub_node(cdk::sub_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.SUB();
}
void til::postfix_writer::do_mul_node(cdk::mul_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MUL();
}
void til::postfix_writer::do_div_node(cdk::div_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.DIV();
}
void til::postfix_writer::do_mod_node(cdk::mod_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void til::postfix_writer::do_lt_node(cdk::lt_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
}
void til::postfix_writer::do_le_node(cdk::le_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}
void til::postfix_writer::do_ge_node(cdk::ge_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}
void til::postfix_writer::do_gt_node(cdk::gt_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
}
void til::postfix_writer::do_ne_node(cdk::ne_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}
void til::postfix_writer::do_eq_node(cdk::eq_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.INT(0);
  _pf.EQ();
}

void til::postfix_writer::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;
  node->left()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.GT();
  _pf.DUP32();
  _pf.JZ(mklbl(lbl = ++_lbl));
  node->right()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.GT();
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void til::postfix_writer::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;
  node->left()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.GT();
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl = ++_lbl));
  node->right()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.GT();
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (symbol->global()) {
    _pf.ADDR(symbol->name());
    // _pf.ADDR(node->name());
  } 
  else {
    _pf.LOCAL(symbol->offset());
  }
}

void til::postfix_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_INT)) {
    _pf.LDINT();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDDOUBLE();
  } 
  else if (node->is_typed(cdk::TYPE_STRING)){
    _pf.LDINT();
  }
  else {
    std::cerr << "error: cannot happen" << std::endl;
  }
}

void til::postfix_writer::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl + 2); // determine the new value
  if (node->is_typed(cdk::TYPE_INT)) {
    _pf.DUP32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT))
      _pf.I2D();
    _pf.DUP64();
  }
  else if (node->is_typed(cdk::TYPE_STRING)){
    _pf.DUP32();
  }
  else {
    std::cerr << "error: cannot happen" << std::endl;
  }

  node->lvalue()->accept(this, lvl); // where to store the value
  if (node->is_typed(cdk::TYPE_INT)) {
    _pf.STINT();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.STDOUBLE();
  }
  else if (node->is_typed(cdk::TYPE_STRING)) {
    _pf.STINT();
  }
  else {
    std::cerr << "error: cannot happen" << std::endl;
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_block_node(til::block_node *const node, int lvl) {
  _symtab.push(); // for block-local vars
  if (node->declarations())
    node->declarations()->accept(this, lvl + 2);
  if (node->instructions())
    node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_program_node(til::program_node *const node, int lvl) {
  // Note that Simple doesn't have functions. Thus, it doesn't need
  // a function node. However, it must start in the main function.
  // The ProgramNode (representing the whole program) doubles as a
  // main function node.

  _symtab.push(); // scope of args

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");

  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables

  _offset = 0; // prepare for local variable

  _inFunctionBody = true;
  node->block()->accept(this, lvl);
  _inFunctionBody = false;

  // end the main function
  _pf.INT(0);
  _pf.STFVAL32();
  _pf.LEAVE();
  _pf.RET();

  _symtab.pop(); // scope of arguments

  // these are just a few library function imports
  _pf.EXTERN("readi");
  _pf.EXTERN("printi");
  _pf.EXTERN("printd");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_evaluation_node(til::evaluation_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void til::postfix_writer::do_print_node(til::print_node *const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  // node->argument()->accept(this, lvl); // determine the value to print
  // if (node->argument()->is_typed(cdk::TYPE_INT)) {
  //   _pf.CALL("printi");
  //   _pf.TRASH(4); // delete the printed value
  // } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
  //   _pf.CALL("prints");
  //   _pf.TRASH(4); // delete the printed value's address
  // } else {
  //   std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
  //   exit(1);
  // }
  // _pf.CALL("println"); // print a newline

  for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
    auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));

    child->accept(this, lvl); // expression to print
    std::shared_ptr<cdk::basic_type> etype = child->type();
    if (etype->name() == cdk::TYPE_INT) {
      // _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // trash int
    } else if (etype->name() == cdk::TYPE_DOUBLE) {
      // _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // trash double
    } else if (etype->name() == cdk::TYPE_STRING) {
      // _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    } else {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }

  }

  if (node->newline()) {
    // _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_read_node(til::read_node *const node, int lvl) {
  // ASSERT_SAFE_EXPRESSIONS;
  // _pf.CALL("readi");
  // _pf.LDFVAL32();
  // node->argument()->accept(this, lvl);
  // _pf.STINT();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_loop_node(til::loop_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _loopTest.push_back(++_lbl);
  _loopEnd.push_back(++_lbl);

  _pf.LABEL(mklbl(_loopTest.back()));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(_loopEnd.back()));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(_loopTest.back()));
  _pf.LABEL(mklbl(_loopEnd.back()));

  _loopTest.pop_back();
  _loopEnd.pop_back();
}

void til::postfix_writer::do_stop_node(til::stop_node *const node, int lvl) {
  size_t level = static_cast<size_t>(node->level());

  if (level <= 0) {
    std::cerr << node->lineno() << ": wrong level for 'stop'" << std::endl;
  }
  else if (level > _loopEnd.size()) {
    std::cerr << node->lineno() << ": 'stop' outside 'loop'" << std::endl;
  }
  else {
    _pf.JMP(mklbl(_loopEnd[_loopEnd.size() - level])); // jump to loop end
  }
}

void til::postfix_writer::do_next_node(til::next_node *const node, int lvl) {
  size_t level = static_cast<size_t>(node->level());

  if (level <= 0) {
    std::cerr << node->lineno() << ": wrong level for 'next'" << std::endl;
  }
  else if (level > _loopTest.size()) {
    std::cerr << node->lineno() << ": 'next' outside 'loop'" << std::endl;
  }
  else {
    _pf.JMP(mklbl(_loopTest[_loopTest.size() - level])); // jump to next cycle
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_if_node(til::if_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

void til::postfix_writer::do_if_else_node(til::if_else_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_function_definition_node(til::function_definition_node *const node, int lvl) {
  // TODO
}

void til::postfix_writer::do_function_call_node(til::function_call_node *const node, int lvl) {
  // TODO
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_return_node(til::return_node *const node, int lvl) {
  // TODO
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_variable_declaration_node(til::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->identifier();

  int offset = 0, typesize = node->type()->size(); // in bytes
  if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } else if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize;
  } else {
    offset = 0; // global variable
  }

  auto symbol = new_symbol();
  if (symbol) {
    symbol->offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionBody) {
    // if we are dealing with local variables, then no action is needed
    // unless an initializer exists
    if (node->initializer()) {
      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      }
      else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        // TODO
      }
      else if (node->is_typed(cdk::TYPE_STRING)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT(); 
      }
      else {
        std::cerr << "cannot initialize" << std::endl;
      }
    }
  }
  else {
    if (node->qualifier() == tEXTERNAL) {
      _pf.EXTERN(id);
    }
    else if (node->qualifier() == tFORWARD) {
      // TODO
    }
    else {
      if (node->initializer()) {
        if (node->is_typed(cdk::TYPE_INT)) {
          _pf.DATA();
          _pf.ALIGN();
          if (node->qualifier() == tPUBLIC)
            _pf.GLOBAL(id, _pf.OBJ());
          _pf.LABEL(id);
          node->initializer()->accept(this, lvl);
        }
        else if (node->is_typed(cdk::TYPE_DOUBLE)) {
          _pf.DATA();
          _pf.ALIGN();
          if (node->qualifier() == tPUBLIC)
            _pf.GLOBAL(id, _pf.OBJ());
          _pf.LABEL(id);

          if (node->initializer()->is_typed(cdk::TYPE_INT)) {
            cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
            cdk::double_node ddi(dclini->lineno(), dclini->value());
            ddi.accept(this, lvl);
          }
          else if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
            node->initializer()->accept(this, lvl);
          }
          else {
            std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
          }
        }
        else if (node->is_typed(cdk::TYPE_STRING)) {
          _pf.DATA();
          _pf.ALIGN();
          if (node->qualifier() == tPUBLIC)
            _pf.GLOBAL(id, _pf.OBJ());
          _pf.LABEL(id);
          node->initializer()->accept(this, lvl);
        }
        else {
          std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer" << std::endl;
        }
      }
      else {
        _pf.BSS();
        _pf.ALIGN();
        if (node->qualifier() == tPUBLIC)
          _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
        _pf.SALLOC(node->type()->size());
      }
    }
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_nullptr_node(til::nullptr_node *const node, int lvl) {
  // TODO
}

void til::postfix_writer::do_index_node(til::index_node *const node, int lvl) {
  // TODO
}

void til::postfix_writer::do_stack_alloc_node(til::stack_alloc_node *const node, int lvl) {
  // TODO
}

void til::postfix_writer::do_address_of_node(til::address_of_node *const node, int lvl) {
  // TODO
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_sizeof_node(til::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
}
