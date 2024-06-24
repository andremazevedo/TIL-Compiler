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

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    auto node_type = cdk::reference_type::cast(node->type());
    _pf.INT(node_type->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    auto node_type = cdk::reference_type::cast(node->type());
    _pf.INT(node_type->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DADD();
  else
    _pf.ADD();
}

void til::postfix_writer::do_sub_node(cdk::sub_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    auto node_type = cdk::reference_type::cast(node->type());
    _pf.INT(node_type->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }
  else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    auto node_type = cdk::reference_type::cast(node->type());
    _pf.INT(node_type->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DSUB();
  else
    _pf.SUB();

  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    auto left_type = cdk::reference_type::cast(node->left()->type());
    _pf.INT(left_type->referenced()->size());
    _pf.DIV();
  }
}

void til::postfix_writer::do_mul_node(cdk::mul_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DMUL();
  else
    _pf.MUL();
}

void til::postfix_writer::do_div_node(cdk::div_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE))
    _pf.DDIV();
  else
    _pf.DIV();
}

void til::postfix_writer::do_mod_node(cdk::mod_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  _pf.MOD();
}

void til::postfix_writer::do_lt_node(cdk::lt_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LT();
}

void til::postfix_writer::do_le_node(cdk::le_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LE();
}

void til::postfix_writer::do_ge_node(cdk::ge_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GE();
}

void til::postfix_writer::do_gt_node(cdk::gt_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GT();
}

void til::postfix_writer::do_ne_node(cdk::ne_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.NE();
}

void til::postfix_writer::do_eq_node(cdk::eq_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    _pf.I2D();

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.EQ();
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
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

  if (symbol->is_typed(cdk::TYPE_FUNCTIONAL)) {
    set_function_symbol(symbol); // advise that a function symbol has been found
  }
  else if (symbol->global()) {
    _pf.ADDR(symbol->name());
  } 
  else {
    _pf.LOCAL(symbol->offset());
  }
}

void til::postfix_writer::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
    node->lvalue()->accept(this, lvl);
    // no action needed
  }
  else {
    node->lvalue()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.LDDOUBLE();
    }
    else {
      _pf.LDINT();
    }
  }
}

void til::postfix_writer::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
    node->rvalue()->accept(this, lvl + 2); // determine the new function

    auto rvalue_function = function_symbol();
    if (rvalue_function) {
      if (rvalue_function->global()) {
        _pf.ADDR(rvalue_function->name());
      }
      else {
        _pf.LOCAL(rvalue_function->offset());
        _pf.LDINT();
      }
      reset_function_symbol();
    }

    node->lvalue()->accept(this, lvl); // where to store the value

    auto lvalue_function = function_symbol();
    if (lvalue_function)
      reset_function_symbol();

    if (lvalue_function->global()) {
      lvalue_function->set_name(rvalue_function->name());
    }
    else {
      _pf.DUP32();
      _pf.LOCAL(lvalue_function->offset());
      _pf.STINT();
    }
  }
  else {
    node->rvalue()->accept(this, lvl + 2); // determine the new value
    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (node->rvalue()->is_typed(cdk::TYPE_INT))
        _pf.I2D();
      _pf.DUP64();
    }
    else {
      _pf.DUP32();
    }

    node->lvalue()->accept(this, lvl); // where to store the value
    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.STDOUBLE();
    }
    else {
      _pf.STINT();
    }
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
  ASSERT_SAFE_EXPRESSIONS;

  auto function = new_symbol();
  // _functions_to_declare.erase("_main");  // just in case
  reset_new_symbol();
  _functions.push(function);

  _bodyRetLabel.push(++_lbl);

  _symtab.push(); // scope of args

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");

  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab, _functions);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables

  _offset = 0; // prepare for local variable

  _inFunctionBody++;
  os() << "        ;; before body " << std::endl;
  node->block()->accept(this, lvl);
  os() << "        ;; after body " << std::endl;
  _inFunctionBody--;

  // end the main function
  _pf.LABEL(mklbl(_bodyRetLabel.top()));
  _pf.LEAVE();
  _pf.RET();
  _bodyRetLabel.pop();

  _symtab.pop(); // scope of arguments

  _functions.pop();

  // declare external functions
  for (std::string s : _functions_to_declare)
    _pf.EXTERN(s);
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_evaluation_node(til::evaluation_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.TRASH(node->argument()->type()->size());
}

void til::postfix_writer::do_print_node(til::print_node *const node, int lvl) {
  for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
    auto argument = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));

    argument->accept(this, lvl); // expression to print
    if (argument->is_typed(cdk::TYPE_INT)) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // trash int
    } 
    else if (argument->is_typed(cdk::TYPE_DOUBLE)) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // trash double
    } 
    else if (argument->is_typed(cdk::TYPE_STRING)) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    } 
    else {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }

  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_read_node(til::read_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  }
  else {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  }
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
  ASSERT_SAFE_EXPRESSIONS;

  auto function = function_symbol();
  if (function)
    reset_function_symbol();
  else
    function = til::make_symbol(node->type(), mklbl(++_lbl), tPRIVATE);

  _functions.push(function);

  int functionEndLabel = ++_lbl;

  _pf.JMP(mklbl(functionEndLabel));

  _symtab.push(); // scope of args

  _bodyRetLabel.push(++_lbl);

  _offset = 8; // prepare for arguments (4: remember to account for return address)

  _inFunctionArgs++;
  if (node->arguments())
    node->arguments()->accept(this, lvl + 4);
  _inFunctionArgs--;

  _pf.TEXT();
  _pf.ALIGN();
  if (function->qualifier() == tPUBLIC)
    _pf.GLOBAL(function->name(), _pf.FUNC());
  _pf.LABEL(function->name());

  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab, _functions);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables

  _offset = 0; // prepare for local variable

  _inFunctionBody++;
  os() << "        ;; before body " << std::endl;
  node->block()->accept(this, lvl + 2);
  os() << "        ;; after body " << std::endl;
  _inFunctionBody--;

  _pf.LABEL(mklbl(_bodyRetLabel.top()));
  _pf.LEAVE();
  _pf.RET();
  _bodyRetLabel.pop();

  _symtab.pop(); // scope of arguments

  _pf.LABEL(mklbl(functionEndLabel));

  _functions.pop();

  set_function_symbol(function); // advise that a function symbol has been defined
}

void til::postfix_writer::do_function_call_node(til::function_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::shared_ptr<til::symbol> function;

  if (node->expression()) {
    node->expression()->accept(this, lvl + 2);

    function = function_symbol();
    if (function)
      reset_function_symbol();
  }
  else {
    // @ recursive function call
    function = _functions.top();
  }

  auto function_type = cdk::functional_type::cast(function->type());

  int argsSize = 0;
  if (node->arguments()) {
    os() << "        ;; before arguments " << std::endl;
    for (int i = node->arguments()->size() - 1; i >= 0; i--) {
      auto argument = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));

      if (function_type->input(i)->name() == cdk::TYPE_FUNCTIONAL) {
        argument->accept(this, lvl + 2);

        auto argument_function = function_symbol();
        if (argument_function)
          reset_function_symbol();

        if (argument_function->global())
          _pf.ADDR(argument_function->name());
        else
          _pf.LOCAL(argument_function->offset());
      }
      else {
        argument->accept(this, lvl + 2);
        if (function_type->input(i)->name() == cdk::TYPE_DOUBLE && argument->is_typed(cdk::TYPE_INT))
          _pf.I2D();
      }

      argsSize += function_type->input(i)->size();
    }
    os() << "        ;; after arguments " << std::endl;
  }

  if (function->qualifier() == tEXTERNAL) {
    _pf.CALL(function->name());

    if (argsSize)
      _pf.TRASH(argsSize);

    if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.LDFVAL64();
    }
    else if (!node->is_typed(cdk::TYPE_VOID)) {
      _pf.LDFVAL32();
    }
  }
  else {
    if (function->global()) {
      _pf.ADDR(function->name());
    }
    else {
      _pf.LOCAL(function->offset());
      _pf.LDINT();
    }

    _pf.BRANCH();  

    if (argsSize)
      _pf.TRASH(argsSize);

    if (node->is_typed(cdk::TYPE_INT)) {
      _pf.LDFVAL64();
      _pf.D2I();
    }
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _pf.LDFVAL64();
    }
    else if (!node->is_typed(cdk::TYPE_VOID)) {
      _pf.LDFVAL32();
    }
  }
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_return_node(til::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto function_type = cdk::functional_type::cast(_functions.top()->type());

  // should not reach here without returning a value (if not void)
  if (function_type->output(0)->name() != cdk::TYPE_VOID) {
    if (function_type->output(0)->name() == cdk::TYPE_FUNCTIONAL) {
      node->retval()->accept(this, lvl);

      auto function = function_symbol();
      if (function)
        reset_function_symbol();

      if (function->global()) {
        _pf.ADDR(function->name());
      }
      else {
        _pf.LOCAL(function->offset());
        _pf.LDINT();
      }

      _pf.STFVAL32();
    }
    else {
      node->retval()->accept(this, lvl);

      if (function_type->output(0)->name() == cdk::TYPE_INT) {
        _pf.I2D();
        _pf.STFVAL64();
      }
      else if (function_type->output(0)->name() == cdk::TYPE_DOUBLE) {
        if (node->retval()->is_typed(cdk::TYPE_INT))
          _pf.I2D();
        _pf.STFVAL64();
      }
      else if (function_type->output(0)->name() == cdk::TYPE_STRING) {
        _pf.STFVAL32();
      }
      else if (function_type->output(0)->name() == cdk::TYPE_POINTER) {
        _pf.STFVAL32();
      }
      else {
        std::cerr << node->lineno() << ": unknown return type" << std::endl;
      }
    }
  }

  _pf.JMP(mklbl(_bodyRetLabel.top()));
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_variable_declaration_node(til::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->identifier();

  int offset, typesize = node->type()->size(); // in bytes
  if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize;
  } 
  else if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } 
  else {
    offset = 0; // global variable
  }

  auto symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionArgs) {
    // if we are dealing with function arguments, then no action is needed
  }
  else if (_inFunctionBody) {
    // if we are dealing with local variables, then no action is needed
    // unless an initializer exists
    if (node->initializer()) {
      if (node->is_typed(cdk::TYPE_INT)) {
        node->initializer()->accept(this, lvl);
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      }
      else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        node->initializer()->accept(this, lvl);
        if (node->initializer()->is_typed(cdk::TYPE_INT))
          _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      }
      else if (node->is_typed(cdk::TYPE_STRING)) {
        node->initializer()->accept(this, lvl);
        _pf.LOCAL(symbol->offset());
        _pf.STINT(); 
      }
      else if (node->is_typed(cdk::TYPE_POINTER)) {
        node->initializer()->accept(this, lvl);
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      }
      else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
        node->initializer()->accept(this, lvl);

        auto function = function_symbol();
        if (function) {
          if (function->global()) {
            _pf.ADDR(function->name());
          }
          else {
            _pf.LOCAL(function->offset());
            _pf.LDINT();
          }
          reset_function_symbol();
        }

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
      _functions_to_declare.insert(id);
    }
    else if (node->qualifier() == tFORWARD) {
      _functions_to_declare.insert(id);
    }
    else {
      _functions_to_declare.erase(id);  // just in case

      if (node->initializer()) {
        // The initialization expression must be a literal or functional value if the variable is global.
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
        else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
          set_function_symbol(symbol);

          node->initializer()->accept(this, lvl);

          auto function = function_symbol();
          if (function)
            reset_function_symbol();
        }
        else {
          std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer" << std::endl;
        }
      }
      else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
        // if we are dealing with a function declaration without an initializer, then no action is needed
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
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody)
    _pf.INT(0);
  else
    _pf.SINT(0);
}

void til::postfix_writer::do_index_node(til::index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->base()->accept(this, lvl + 2);
  node->index()->accept(this, lvl + 2);
  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}

void til::postfix_writer::do_stack_alloc_node(til::stack_alloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto alloc_type = cdk::reference_type::cast(node->type());

  node->argument()->accept(this, lvl + 2);
  _pf.INT(alloc_type->referenced()->size());
  _pf.MUL();
  _pf.ALLOC(); // allocate
  _pf.SP(); // put base pointer in stack
}

void til::postfix_writer::do_address_of_node(til::address_of_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // since the argument is an lvalue, it is already an address
  node->lvalue()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void til::postfix_writer::do_sizeof_node(til::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
}
