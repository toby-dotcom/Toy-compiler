# Simple Educational Compiler

A minimal compiler written in C++ for educational purposes.

This project implements a basic compilation pipeline:
source code → tokens → Abstract Syntax Tree (AST)

---

##  Features

-  Lexical analysis (tokenization)
-  Syntax analysis (AST construction)
-  Supported language constructs:
  - Variable declaration (`int x = 42;`)
  - Assignment (`x = x + 5;`)
  - `if-else` statements
  - `while` loops
  - Function calls (`print("Hello");`)
  - Binary operations (`+`, `-`, `*`, `/`, `==`, `<`, `>`)

---

##  Requirements

- C++11 or newer
- g++, clang, or MSVC

---

##  Build & Run

```bash
g++ -std=c++11 lexer.cpp parser.cpp main.cpp -o compiler
./compiler
```

---

##  Example

### Input

```cpp
int x = 42;

if (x > 10) {
    x = x + 1;
    print("Hello!");
}
```

### Output (AST)

```
VarDecl(int x = Number(42))

If((Identifier(x) > Number(10)))
  Assignment(x = (Identifier(x) + Number(1)))
  Call(print, [String("Hello!")])
```

---

##  Purpose

This project is designed to demonstrate core compiler concepts:
lexical analysis, parsing, and AST construction.
It is intended for learning and experimentation.

