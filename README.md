# Compiler-Construction


This repository contains code for a compiler of a custom language. The different modules of the compiler implemented are: 

1. Lexer
2. Parser
3. Abstract Syntax Tree Generator
4. Typechecker and Semantic Analyser
5. Code Generator

## Input
The driver function can be found in `driver.c`. It expects a code.txt file, a grammar.txt file and which modules to run as command line arguments. The grammar is expected to be LL(1) compatible. Alternatively, a makefile is also present in which the default target runs all modules together. A `code.asm` file is generated which can be run using NASM-64. 

## Lexer
The lexer outputs a line-wise list of tokens of the language. The list of tokens can be found in `lexer.c`. It also points out the errors in lexing the input, as `invalid token on line 64`

## Parser
The parser converts the tokens given by the lexer into a parse tree. The parse tree contains redundant nodes as well, which are later removed by the semantic phase. The output of the parser can be seen as an output on the terminal as well. 

## Semantics
The semantic analyser conver the parse tree into an AST. The AST can also be seen as an output on the command line. Some advanced error detection techniques are also used here. The synchronization set can be easily modified in the `AST.c` file. 

