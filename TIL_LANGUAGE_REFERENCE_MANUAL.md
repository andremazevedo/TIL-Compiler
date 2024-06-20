# TIL Language Reference Manual

## Contents

1. [Data Types](#data-types)
2. [Name Manipulation](#name-manipulation)
    1. [Namespace and Identifier Visibility](#namespace-and-identifier-visibility)
    2. [Variable Validity](#variable-validity)
3. [Lexical Conventions](#lexical-conventions)
    1. [Whitespace Characters](#whitespace-characters)
    2. [Comments](#comments)
    3. [Keywords](#keywords)
    4. [Types](#types)
    5. [Expression Operators](#expression-operators)
    6. [Delimiters and Terminators](#delimiters-and-terminators)
    7. [Identifiers (Names)](#identifiers-names)
    8. [Literals](#literals)
        1. [Integers](#integers)
        2. [Floating Point Numbers](#floating-point-numbers)
        3. [Character Strings](#character-strings)
        4. [Pointers](#pointers)
4. [Grammar](#grammar)
    1. [Types, Identifiers, Literals, and Expression Definition](#types-identifiers-literals-and-expression-definition)
    2. [Left-values](#left-values)
    3. [Files](#files)
    4. [Variable Declaration](#variable-declaration)
    5. [Global Symbols](#global-symbols)
    6. [Initialization](#initialization)
5. [Functions](#functions)
    1. [Declaration](#declaration)
    2. [Invocation](#invocation)
    3. [Body](#body)
    4. [Main Function and Program Execution](#main-function-and-program-execution)
6. [Instructions](#instructions)
    1. [Blocks](#blocks)
    2. [Conditional Instruction: if](#conditional-instruction-if)
    3. [Iteration Instruction: loop](#iteration-instruction-loop)
    4. [Termination Instruction](#termination-instruction)
    5. [Continuation Instruction](#continuation-instruction)
    6. [Return Instruction](#return-instruction)
    7. [Expressions as Instructions](#expressions-as-instructions)
    8. [Print Instructions](#print-instructions)
7. [Expressions](#expressions)
    1. [Primitive Expressions](#primitive-expressions)
        1. [Identifiers](#identifiers)
        2. [Reading](#reading)
        3. [Functions](#functions-2)
    2. [Expressions Resulting from Operator Evaluation](#expressions-resulting-from-operator-evaluation)
        1. [Pointer Indexing](#pointer-indexing)
        2. [Identity and Symmetric](#identity-and-symmetric)
        3. [Memory Allocation](#memory-allocation)
        4. [Position Indication Expression](#position-indication-expression)
        5. [Dimension Expression](#dimension-expression)
8. [Examples and Tests](#examples-and-tests)
9. [Omissions and Errors](#omissions-and-errors)

Este manual apresenta características básicas da linguagem TIL (tipos de dados, manipulação de nomes); convenções lexicais; estrutura/sintaxe; especificação das funções; semântica das instruções; semântica das expressões; e, finalmente, alguns exemplos.

# Data Types

The language is weakly typed (some implicit conversions are performed). There are 4 basic data types, all compatible with the [C programming language](https://en.wikipedia.org/wiki/C_(programming_language)), and always memory-aligned at 32 bits:

- Numeric types: integers, in two's complement, occupy 4 bytes; reals, in [floating point](https://en.wikipedia.org/wiki/Floating_point), occupy 8 bytes ([IEEE 754](https://en.wikipedia.org/wiki/IEEE_floating_point)).
- Character strings are vectors of characters terminated by [ASCII](https://en.wikipedia.org/wiki/ASCII) NUL (character with the value zero). Variables and literals of this type can only be used in assignments, prints, or as function arguments/returns. Characters are 8-bit values that are not directly manipulable.
- Pointers represent object addresses and occupy 4 bytes. They can be subjected to arithmetic operations (shifts) and allow access to the pointed value.

The types supported by each operator and the operation to be performed are indicated in the expression definitions.

There are also types associated with functional values, i.e., types that describe the function interface (see below). Memory values associated with these types are effectively pointers, but for functions and not for data, and can be used to invoke the corresponding functions. These pointers do not accept pointer arithmetic or indexing operations (although pointers to these pointers do).

# Name Manipulation

Names (identifiers) correspond exclusively to variables. Functions are referenced through named pointers. In the following points, the term entity is used to indiscriminately designate them, specifying when the description is valid only for one of the cases. There are also names for external functions: in this case, the names directly refer to the position of the code of these functions (as in C; see **external** below).

## Namespace and Identifier Visibility

The global namespace is unique, so a name used to designate an entity in a given context cannot be used to designate others (even if of a different nature).

Identifiers are visible from the declaration until the end of the scope: file (global) or block (local). Reusing identifiers in lower contexts obscures declarations in higher contexts: local redeclarations can obscure global ones until the end of a block, for example. It is possible to use global symbols in the block contexts of functions, but it is not possible to declare them (see [global symbols](#global-symbols)).

## Variable Validity

Global entities (declared outside any function) exist throughout the program's execution. Local variables to a function exist only during its execution. Formal arguments are valid while the function is active.

# Lexical Conventions

For each group of lexical elements (tokens), the longest sequence of characters constituting a valid element is considered. Thus, for example, the designation **>=** is always a single lexical element (as opposed to the illegal situation of having two symbols: **>** followed by **=**).

## White Characters

Are considered separators and do not represent any lexical element: **newline** ASCII LF (**0x0A**, **\n**), **carriage return** ASCII CR (**0x0D**, **\r**), **space** ASCII SP (**0x20**, &ensp;), and **horizontal tab** ASCII HT (**0x09**, **\t**).

## Comments

There are two types of comments, which also function as separator elements:

- **Explanatory**: start with **;** and end at the end of the line; and
- **Operational**: start with **/** and end with **/**, and can be nested.

If the start sequences are part of a character string, they do not start a comment (see [character string definition](#character-strings)).

## Keywords

The following keywords are reserved and do not constitute identifiers (must be written exactly as indicated):

- Types: **int double string void**
- Declarations: **external forward public var**
- Instructions: **block if loop stop next return print println**
- Expressions: **read null set index objects sizeof function**
- Others: **program**

## Types

The following lexical elements designate types in declarations (see grammar): **int** (integer), **double** (real), **string** (character string).

The types corresponding to pointers are other types followed by **!**, designating an indirection and not the direct object (see grammar).

## Expression Operators

The elements presented in the expression definitions are considered operators.

## Delimiters and Terminators

The following lexical elements are delimiters/terminators:

- **(** and **)** (expression delimiters)
- **"** (character string delimiter)

## Identifiers (names)

Are initiated by a letter, followed by 0 (zero) or more letters or decimal digits. The length of the name is unlimited and two names are distinct if there is a change from uppercase to lowercase, or vice versa, of at least one character.

## Literals

Are notations for constant values of some language types (not to be confused with constants, i.e., identifiers that, in some languages, designate elements whose value cannot be changed during the program execution).

### Integers

An integer literal is a non-negative number. Negative numbers can be constructed by applying the unary negation operator (**-**) to a literal (always positive).

Decimal integer literals consist of sequences of 1 (one) or more digits from **0** to **9**, where the first digit is not 0 (zero), except in the case of the number 0 (zero). In this case, it is composed only of the digit 0 (zero) (in any base).

Base 16 integer literals always start with the sequence **0x**, followed by one or more digits from **0** to **f**, with no difference between uppercase and lowercase (note that **0xATP7** is an invalid base 16 literal). Examples (valid): **0x07**, **0xabc**, **0xAbC**.

If it is not possible to represent an integer literal on the machine, due to overflow, a lexical error should be generated.

### Floating Point Reals

Real literals (always positive) are expressed as in C (exclusively in base 10).

There are no negative literals (negative numbers result from the unary operation **-**).

Note that a literal without **.** (decimal point) or exponential part is of type integer.

Examples: **3.14**, **1E3** = 1000 (integer number represented in floating point). **12.34e-24** = 12.34 x 10<sup>-24</sup> (scientific notation).

If it is not possible to represent a double literal on the machine, due to overflow, a lexical error should be generated.

### Character Strings

Character strings are delimited by quotes (**"**) and can contain any characters, except ASCII NUL (0x00) and ASCII LF (0x0A). In strings, comment delimiters have no special meaning. If a literal containing **\0** is written, then the string ends at that position. Example: **ab\0xy** has the same meaning as **ab**.

It is possible to designate characters by special sequences (initiated by **\\**), especially useful when there is no direct graphical representation. The special sequences correspond to the ASCII characters HT, LF, and CR (**\\t**, **\\n**, and **\\r**, respectively), quote (**\\"**), backslash (**\\\\**), or any others specified through 1 to 3 digits in base 8, designating 8-bit values (e.g., **\012** or just **\12** if the next character does not represent a base 8 digit). Example: **xy\012z** has the same meaning as **xy\12z** and **xy\nz**.

### Pointers

The only admissible literal for pointers corresponds to the null pointer and is indicated by the keyword **null**. This literal is compatible with all pointer types. Integers are not convertible to pointers, so the value **0** (zero) is not an admissible initial value for pointers.

# Grammar

The grammar of the language is summarized below. It is considered that elements in fixed type are literals, that curved parentheses group elements, that alternative elements are separated by a vertical bar, that optional elements are in square brackets, and that elements that repeat zero or more times are between ⟨ and ⟩. Some elements used in the grammar are also elements of the language described if represented in fixed type (e.g., parentheses).

|                           |       |                                                        |
|---------------------------|-------|---------------------------------------------------------|
| *file*                    | → | ⟨ *declaration* ⟩ [ *main-program* ]                    |
| *declaration*             | → | **(** [ *qualifier* ] *type* *identifier* [ *expression* ] **)** |
|                           | → | **(** [ *qualifier* ] [ **var** ] *identifier* *expression* **)** |
| *declarations*            | → | *declaration* ⟨ *declaration* ⟩                         |
| *main-program*            | → | **(** **program** ⟨ *declaration* ⟩ ⟨ *instruction* ⟩ **)** |
| *function*                | → | **(** **function** **(** *type* **)** ⟨ *declaration* ⟩ ⟨ *instruction* ⟩ **)** |
|                           | → | **(** **function** **(** *type* *declarations* **)** ⟨ *declaration* ⟩ ⟨ *instruction* ⟩ **)** |
| *type*                    | → | **int** | **double** | **string** | **void** | *type* **!** | *function-type* |
| *function-type*           | → | **(** *type* **)**                                    |
|                           | → | **(** *type* **(** *types* **)** **)**               |
| *types*                   | → | *type* ⟨ *type* ⟩                                     |
| *block*                   | → | **(** **block** ⟨ *declaration* ⟩ ⟨ *instruction* ⟩ **)** |
| *instruction*             | → | *expression* | **(** **print** *expressions* **)** | **(** **println** *expressions* **)** |
|                           | → | **(** **stop** [ *integer-literal* ] **)** | **(** **next** [ *integer-literal* ] **)** | **(** **return** [ *expression* ] **)** |
|                           | → | *conditional-instruction* | *iteration-instruction* | *block* |
| *conditional-instruction* | → | **(** **if** *expression* *instruction* [ *instruction* ] **)** |
| *iteration-instruction*   | → | **(** **loop** *expression* *instruction* **)**       |
| *expressions*             | → | *expression* ⟨ *expression* ⟩                         |

## Types, Identifiers, Literals, and Expression Definition

Some definitions were omitted from the grammar: qualifiers and variable (see [variable declarations](#variable-declaration)), identifier (see [identifiers](#identifiers-names)), literal (see [literals](#literals)), expression (see [expressions](#expressions)). Note that *function* refers to any function specification (body or pointer with the appropriate type). In this sense, functions count as primitive expressions.

Regarding data types, **int** designates integer values, **double** designates real values, **string** designates character strings. Pointer types are composed types followed by **!**, e.g., **int!** (pointer to integer) or **(double)!** (pointer to function with no arguments returning a floating-point value).

Pointers to functions are defined from the previously described data types and, additionally, by the special type **void** (see below). The type of a function is indicated as specified in the grammar, i.e., return type followed by the argument types (zero or more, as described).

A pointer declared with a function type indicates the address of the corresponding function. These pointers do not support pointer arithmetic. Note that a pointer to a function pointer behaves like a pointer to a data area and thus supports pointer arithmetic.

The type **void** can only be used to indicate an absence of return value or to declare a generic pointer. In this case, nesting is irrelevant, i.e., **void!** and **void!!!** are equivalent. A pointer of this type is compatible with all other pointer types. Pointer arithmetic increments/decrements the value of a **void!** pointer by one unit.

## Left-values

Left-values are memory positions that can be modified (except where prohibited by the data type). The elements of an expression that can be used as left-values are individually identified in the semantics of expressions.

## Files

A file is designated as the main if it contains the main function (where the program starts). This function is explicitly designated by using the keyword **program** (see [Main function and program execution](#main-function-and-program-execution)).

## Variable Declaration

A variable declaration always indicates a data type (implicit or explicit) and an identifier.

Examples:
- Integer: **(int i)**
- Real: **(double r)**
- String: **(string s)**
- Pointer to integer: **(int! p1)** (equivalent to **int*** in C)
- Pointer to real: **(double! p2)** (equivalent to **double*** in C)
- Pointer to string: **(string! p3)** (equivalent to **char*** in C)
- Pointer to pointer to integer: **(int!! p4)** (equivalent to **int*** in C)

It is possible to use the pseudo-type **var** if the declaration has an initial value: in this case, the type is that of the initial value.

Examples:
- Integer variable: **(var i 1)**
- Real variable: **(var f 2.0)**
- etc.

## Global Symbols

By default, symbols are private to a module and cannot be imported by other modules.

The keyword **public** allows declaring an identifier as public, making it accessible from other modules. When used with the keyword **var**, it is optional. Note that the declaration of a variable must always have either the qualifier or the type, with both possibly being present.

The keyword **forward** allows declaring variables defined in other modules within a module. In this case, the initial value of these variables cannot be specified, so it is not possible to use **var** together with **forward**.

The keyword **external** should be used to declare function symbols with a different calling convention from the TIL language, e.g., to import functions defined in C. Besides being used to call the functions they designate, the symbols thus declared can be assigned to variables with the appropriate type.

Examples:
- Declare private variable to module: **(double d1 22.0)**
- Declare public variable: **(public double d2 7.0)**
- Declare public variable: **(public var d2 7.0)** (same as the previous)
- Declare public variable: **(public d2 7.0)** (same as the previous)
- Use external definition of public variable: **(forward double d2)**
- Use pointer to externally defined function (native TIL): **(forward (int (int)) factorial)**
- Use function defined in another language (e.g., C): **(external (void! (int)) malloc)**

## Initialization

When present, it is the designation of the object that follows the variable name in a declaration: integer, real, string, pointer, or function. Real entities can be initialized by integer expressions (implicit conversion). The initialization expression must be a literal or functional value if the variable is global. Functional value assignments to variables can be made when the types are covariant.

The word **var** can be used instead of the type to declare a variable. When used, the type of the variable is inferred from the initial value (in these cases, the initial value is mandatory).

Examples:
- Integer (literal): **(int i 3)**
- Integer (expression): **(int i (+ j 1))**
- Real (literal): **(double r 3.2)**
- Real (expression): **(double r (- i 2.5))**
- String (literal): **(string s "olá")**
- Pointer (literal): **(double!!! p null)**
- Pointer (expression): **(int! p (+ q 1))**
- Function:
```
((int (int)) f1)
((int (double)) g1)
((double (int)) g2)
((int (int)) f2 f1) ; ok: same type
(set f2 g1) ; ok: covariant types
(set f2 g2) ; WRONG
```

# Functions

A function allows grouping a set of instructions in a body, executed based on a set of parameters (the formal arguments), when invoked from an expression.

## Declaration

Functions are anonymous but always referenced by identifiers or pointers, just like other data types. The return type of a function that does not produce return values is **void**.

Functions that receive arguments must indicate them in the header. Functions without arguments define an empty header. The export/import qualifiers **public** or **forward** (see [global symbols](#global-symbols)) apply to the variables used to refer to functions. They cannot be applied to the declarations of a function's arguments. It is not possible to specify default values for a function's arguments or return value.

The declaration of a variable of type function without initialization is used to characterize an external identifier or to make forward declarations (used to pre-declare functions that are used before being defined, for example, between two mutually recursive functions).

## Invocation

A function can be invoked through any expression (pointer) of the appropriate type that refers to that function (non-null pointer). The symbol **@** can be used within the function itself for recursive invocation. It is not possible to use **@** in the main program.

If there are arguments, in the function invocation, the pointer is followed by a list of expressions (the pointer and the arguments are enclosed in parentheses). The sequence of arguments can be empty. The number and type of actual parameters must match the number and type of the formal parameters of the invoked function (subject to implicit conversions). The order of actual parameters should be the same as the formal arguments of the function being invoked.

According to the Cdecl convention, the calling function places the arguments on the stack and is responsible for their removal after the call returns. Thus, the actual parameters should be placed on the stack in reverse order of their declaration (i.e., they are evaluated from right to left before the function invocation, and the result is passed by copy/value). The return address is placed at the top of the stack by the function call.

## Body

The body of a function consists of an implicit block containing (optional) declarations followed by (optional) instructions. It is not possible to apply the export qualifiers (**public**) or import qualifiers (**forward** or **external**) (see [global symbols](#global-symbols)) within the body of a function.

A **return** statement causes the function to terminate. The value returned by a function, through an expression used as an argument of the **return** statement, must be of the type declared in the function header.

It is an error to specify a return value if the function is declared as not returning a value (indicated as **void**).

Any block (used, for example, in a conditional or iteration statement) can define variables, whose values can be other functions. Functions defined within a block do not have access to variables in the current function context where they occur.

## Main Function and Program Execution

A program is defined by the **program** clause. This sequence forms what is called the main function in some languages. The arguments with which the program was called can be obtained through the functions **argc** (returns the number of arguments); **argv** (returns the n-th argument as a string, with n > 0); and **envp** (returns the n-th environment variable as a string, with n > 0). Only one of the program's modules can define the main function, i.e., if there are multiple modules, only one of them can contain more than declarations of global variables.

The return value of the main function (with 8 bits) is returned to the environment that invoked the program. This return value follows the following rules (operating system): 0 (zero), successful execution; 1 (one), invalid arguments (in number or value); 2 (two), execution error. Values above 128 indicate that the program terminated with a signal. Generally, for proper operation, programs should return 0 (zero) if execution was successful and a value different from 0 (zero) in case of error.

```
(external (int) argc)
(external (string (int)) argv)
(external (string (int)) envp)
```

The run-time library (RTS) contains information about other available support functions, including system calls (see also the [RTS Manual](https://www.example.com)).

# Instructions

Unless otherwise indicated, instructions are executed sequentially.

## Blocks

Each block has an area for local variable declarations (optional), followed by an area with instructions (optional). It is possible to define functions within blocks.

The visibility of variables is limited to the block in which they were declared. Declared entities can be directly used in sub-blocks or passed as arguments to functions called within the block. If the identifiers used to define local variables are already being used to define other entities within the scope of the block, the new identifier will refer to a new entity defined within the block until it ends (the previously defined entity continues to exist but cannot be directly referred to by its name). This rule also applies to function arguments (see [function body](#body)).

## Conditional Instruction: if

This instruction behaves similarly to the **if-else** instruction in C: the first instruction is executed if the condition is true; otherwise, if it exists, the second instruction is executed.

## Iteration Instruction: loop

This instruction behaves identically to the **while** instruction in C: the instruction is executed while the condition is true.

## Termination Instruction

The **stop** instruction terminates the n-th innermost loop in which the instruction is found (when the argument is omitted, **n=1** is assumed), similar to the **break** instruction in C. This instruction can only exist within a loop and must be the last instruction of its block.

## Continuation Instruction

The **next** instruction restarts the n-th innermost loop in which the instruction is found (when the argument is omitted, **n=1** is assumed), similar to the **continue** instruction in C. This instruction can only exist within a loop and must be the last instruction of its block.

## Return Instruction

The **return** instruction, if it exists, is the last instruction of its block. See the behavior in the [description of the function body](#body).

## Expressions as Instructions

Expressions used as instructions are always evaluated, even if they do not produce side effects.

## Print Instructions

The **print** and **println** operators allow values to be displayed in the program's output. The first form displays values without changing lines; the second form displays values, changing lines after displaying all of them. When there is more than one expression, the various expressions are displayed without separation. Numeric values (integers or reals) are printed in decimal. Strings are printed in the native encoding. Pointers or other objects cannot be printed.

# Expressions

An expression is an algebraic representation of a quantity: all expressions have a type and return a value.

There are [primitive expressions](#primitive-expressions) and expressions resulting from the [evaluation of operators](#expressions-resulting-from-operator-evaluation).

The following table presents the various operators. Most operators follow the semantics of the C language (except where explicitly indicated). As in C, logical values are 0 (zero) (false value), and non-zero (true value). Note that the explicit delimitation of operators and their arguments makes reference to associativity issues unnecessary, which is needed in other languages. Generally, an expression has the form **(operator arg1 ... argN)**.

| **Expression Type**    | **Operators**      | **Operands**         | **Semantics**                                             |
|------------------------|--------------------|----------------------|-----------------------------------------------------------|
| unary                  | **+ -**            | integers, reals      | [identity and symmetric](#identity-and-symmetric)         |
| unary                  | **?**              | left-values          | [position indication](#position-indication-expression) |
| multiplicative         | **\* / %**         | integers, reals      | C (% is only for integers)                                |
| additive               | **+ -**            | integers, reals, pointers | C: if involving pointers, they calculate: (i) displacements, i.e., one of the operands must be a pointer type and the other an integer type; (ii) pointer differences, i.e., only when applying the **-** operator to two pointers of the same type (the result is the number of objects of the pointed type between them). If memory is not contiguous, the result is undefined. |
| comparative            | **< > <= >=**      | integers, reals      | C                                                         |
| equality               | **== !=**          | integers, reals, pointers | C                                                    |
| logical "not"          | **~**              | integers             | C                                                         |
| logical "and"          | **&&**             | integers             | C: the 2nd argument is only evaluated if the 1st is not false. |
| logical "or"           | **\|\|**           | integers             | C: the 2nd argument is only evaluated if the 1st is not true. |
| assignment             | **set**            | all types            | The value of the expression on the right side of the operator is stored in the position indicated by the *left-value* (left operand of the operator). Integer values can be assigned to real *left-values* (automatic conversion). In other cases, both types must match. The literal **null** is compatible with all pointer types. Example: **(set a 1)** |

## Primitive Expressions

The [literal expressions](#literals) and [function invocation](#invocation) have been defined above.

### Identifiers

An identifier is an expression if it has been declared. An identifier can denote a variable.

An identifier is the simplest case of a *[left-value](#left-values)*, that is, an entity that can be used on the left side of an assignment.

### Reading

The operation of reading an integer or real value is indicated by the operator **read**. This expression returns the value read according to the expected type (integer or real) in the context where it is used. If used as an argument for printing operators or in other situations that allow multiple types (e.g., **print** or **println**), an integer should be read.

Examples: **(set a (read))** (reading into **a**, according to the type of **a**), **(f (read))** (reading as a function argument, according to the type of that argument), **(println (read))** (reading and printing, reading and printing an integer value), **(@ (read))** (recursive function call with an argument read from input, according to the type of the current function's argument).

### Functions

Functions (pointers or their code: not to be confused with function calls) can be used as expressions, typed as functions, i.e., pointers to functions (even when a pointer is not explicitly used).

Example:

```
(var f (function (int (int i)) (return (+ i 1))))
(var g (function (int (int n) ((int (int)) fun)) (return (fun n))))
(program 
  (println (g 3 f))  ; escreve 4
  (println (g 3 (function (int (int i)) (return (* i 2)))))  ; escreve 6
)
```

## Expressions Resulting from Operator Evaluation

### Pointer Indexing

Pointer indexing returns the value at a memory position indicated by a pointer. It consists of an expression designated by the **index** operator and a pointer followed by the index. The result of pointer indexing is a *[left-value](#left-values)*. It is not possible to index pointers that designate functions.

Example (accessing position 0 of the memory area indicated by **p**): **(index p 0)**

### Identity and Symmetric

The identity (**+**) and symmetric (**-**) operators apply to integers and reals. They have the same meaning as in C.

### Memory Allocation

The memory allocation expression returns the pointer that points to the memory area, in the current function's stack, containing enough space for the number of objects indicated by its integer argument.

Example (allocates a vector with 5 reals, pointed to by **p**): **(double! p (objects 5))**

### Position Indication Expression

The **?** operator applies to *left-values*, returning the corresponding address (with the pointer type).

Example (indicates the address of **a**): **(? a)**

### Size Expression

The **sizeof** operator has a single argument and applies to expressions, returning the corresponding size in bytes.

Example: **(sizeof a)** (size of **a**).

# Examples and Tests

The examples below are not exhaustive and do not illustrate all aspects of the language.

Other [test packages](https://gitlab.rnl.tecnico.ulisboa.pt/leic-a-co24/eval/co24) (including evaluation tests) are also available.

The following example illustrates a program with two modules: one that defines the **factorial** function and another that defines the main function.

Definition of the **factorial** function in the file **factorial.til**:

```
(public factorial
        (function (int (int n))
                  (if (> n 1)
                      (return (* n (@ (- n 1))))
                      (return 1)
                  )
        )
)
```

Example of using the **factorial** function in the file **main.til**:

```
; builtin functions from the RTS (non-TIL)
(external (int) argc)
(external (string (int)) argv)
(external (int (string)) atoi)

; user functions from other modules (TIL)
(forward (int (int)) factorial)

; the main function
(program
  (var value 1)
  (println "Teste para a função factorial.")
  (if (== (argc) 2)
    (block
      (var s (argv 1))
      (set value (atoi s))
    )
  )
  (println value "! é " (factorial value))
  (return 0)
)
```

How to compile:

```
til --target asm factorial.til
til --target asm main.til
yasm -felf32 factorial.asm
yasm -felf32 main.asm
ld -melf_i386 -o main factorial.o main.o -lrts
```

# Omissions and Errors

Omissions and errors will be corrected in future versions of the reference manual.
