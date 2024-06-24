# TIL Compiler

This project is a compiler for the TIL language, developed as part of the Compilers course at Instituto Superior Técnico (IST).

The steps to develop a full TIL compiler involved the adaptation of the following components:
- Scanner: `til_scanner.l`
- Parser: `til_parser.y`
- Symbol Table: `targets/symbol.h`
- Type Checker: `targets/type_checker.cpp`
- XML Writer: `targets/xml_writer.cpp`
- Postfix Writer: `targets/postfix_writer.cpp`

For more information about the theoretical topics and the development stages of a compiler, consult [wiki](https://web.tecnico.ulisboa.pt/~david.matos/w/pt/index.php/Compiladores), which contains the course resources.

## Language Reference Manual

- [TIL Language Reference Manual](REFERENCE_MANUAL.md)
- [Manual de Referência da Linguagem TIL](https://web.tecnico.ulisboa.pt/~david.matos/w/pt/index.php/Compiladores/Projecto_de_Compiladores/Projecto_2023-2024/Manual_de_Refer%C3%AAncia_da_Linguagem_TIL) (original, in Portuguese)

## Language Analysis Steps and Tools

The compiler, written in C++, performs the following stages of analysis to implement the language:

### Lexical and Syntactic Analysis

These stages are carried out by the following tools:
- Lexical Analysis: [flex](https://github.com/westes/flex)
- Syntactic Analysis: [bison](https://www.gnu.org/software/bison/)

The code produced by these tools is incorporated by the CDK (Compiler Development Kit) into the compiler, being automatically linked to the subsequent stages.

### Semantic Analysis and Code Generation

These stages are performed by the classes in the CDK library, using the Composite design pattern (for representing the nodes of the AST created during syntactic analysis) and the Visitor design pattern (classes that control the actions of type checking and code generation).

### Assembly Code Compilation to Binary Code

The conversion of the assembly file, produced by the compiler, into a binary module is performed by the tool [yasm](http://yasm.tortall.net/), using the linux/elf32 format.

### Linking Modules into an Executable Image

Linking the binary modules into an executable program is performed by the linker (on Linux, by the GNU ld program, which is included in all distributions).

Besides the modules that carry the programmer's code, translated into machine code, the RTS library is necessary, providing the program with the necessary functions for interaction with the execution environment and the user. For more information, see the [RTS manual](https://web.tecnico.ulisboa.pt/~david.matos/w/pt/index.php/Manual_da_RTS).

## Build TIL Compiler

To build the TIL compiler, follow these steps:

- **Install the required tools**:
  Ensure that `flex`, `bison`, and `yasm` are installed on your system. For a Debian-based system, you can use:
  ```sh
  sudo apt install flex bison yasm
  ```

- **Install the CDK and RTS libraries**:
  The CDK and RTS libraries supporting the development of the project are required.
  ```sh
  wget https://web.tecnico.ulisboa.pt/~ist192424/Libcdk19-202403010000.tar.bz2 
  tar xvf Libcdk19-202403010000.tar.bz2
  cd libcdk19-202403010000
  make install
  ```
  ```sh
  wget https://web.tecnico.ulisboa.pt/~ist192424/Librts5-202103031636.tar.bz2
  tar xvf Librts5-202103031636.tar.bz2
  cd librts5-202103031636
  make install
  ```

- **Clone the repository**:
  ```sh
  git clone https://github.com/andremazevedo/TIL.git
  cd TIL-compiler
  ```

- **Build the compiler**:
  ```sh
  make
  ```

## TIL Program Compilation

To compile and run a TIL program, follow these steps:

- **Compile the source code**:
  Compile the TIL source code into assembly code.
  ```sh
  ./til example.til
  ```

- **Compile the assembly code**:
  Use `yasm` to convert the assembly code into an object file.
  ```sh
  yasm -felf32 -o example.o example.asm
  ```

- **Produce the executable**:
  Link the object file to produce the final executable.
  ```sh
  ld -melf_i386 -o example example.o -L$HOME/compiladores/root/usr/lib -lrts
  ```

- **Run the executable**:
    ```sh
    ./example
    ```

## Automated Tests

To ensure the correctness of the TIL compiler, you can run automated tests provided in the `auto-tests` directory. These tests include example TIL programs and their expected output.

### Run a Single Test

To run a specific test in the `auto-tests` directory.
```sh
./test.sh auto-tests/A-01-1-N-ok.til
```

### Run All Tests

To run all tests in the `auto-tests` directory, use the following command:
```sh
./test.sh
```
