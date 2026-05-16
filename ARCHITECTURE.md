# Architecture Overview

## 1. Project Structure

```text
[thogcc]/
├── include/
│   ├── ast/
│   │   ├── declarations/
│   │   ├── declarators/
│   │   ├── expressions/
│   │   ├── statements/
│   │   ├── nodes.h                     # AST_NODES_ALL macro
│   │   ├── all.h                       # include this instead of individual ast/**.h
│   │   ├── fwd.h                       # Forward definitions. Prefer over all.h
│   │   ├── Node.h                      # Node, VisitableNode, NodeList, other base classes
│   │   └── utils.h                     # NodeKind
│   ├── errors/                         # custom runtime error types
│   ├── types/                          # C typing
│   ├── visitors/
│   ├── ir/
│   │   ├── LLVMModule.h                # internal LLVM representation
│   │   ├── LLVMType.h                  # LLVM typing
│   │   └── helpers.h
│   ├── codegen/
│   ├── cli.h                           # command line arguments
│   ├── CScanner.h                      # scanner interface. wraps flex.
│   └── utils.h                         # utils incl. lambda overload pattern
├── src/
│   ├── visitors/
│   │   ├── PrintVisitor.cpp            # Graph printer (--graph)
│   │   └── SemaVisitor.cpp             # Semantic analysis. Type checking, symbol table, constant folding, etc.
│   │   └── IRGenVisitor.cpp            # AST->LLVM
│   ├── ir/
│   │   └── IREmitter.cpp               # LLVM printer (--emit-llvm)
│   ├── codegen/
│   │   └── RISCVEmitter.cpp            # LLVM->asm (-o)
│   ├── main.cpp
│   ├── cli.cpp                         # argument parsing
│   ├── parse.cpp                       # instantiates scanner+parser
│   ├── lexer.l                         # flex++ specification
│   └── parser.y                        # bison grammar
├── tests/
│   ├── cw-suite/                       # Test suite ported from langproc-cw
│   │   └── _example/
│   │       ├── driver/example_driver.c # Test driver to be fed to gcc
|   |       └── example.c               # Test input to thogcc (-S)
│   └── ours/                           # Expanded test suite by us
└── ARCHITECTURE.md                     # This document
```

## 2. High-Level System Diagram

## 3. Core Components

### 3.1. Frontend (C89/90)

Parses C89 [(see draft spec)](https://port70.net/%7Ensz/c/c89/c89-draft.html) using flex++, bison.

#### 3.1.1. AST representation: `Node`, `VisitableNode`, `NodeList`, `ValueNode`

- Double dispatch visitor pattern (`Node::accept` + `Visitor::visit`)
  - `VisitableNode` CRTP enforces correctness for `accept`.
- `NodeListBase` abstract base class for `NodeList<T>`
- `NodeList<T>`
  - exposes iteration as `begin()`/`end()`
  - type erased at visitor boundary. ie. `NodeList<T>::accept(v)` calls `v.visit(NodeListBase& node)`. This ensures visitors either use iteration or handled `NodeListBase` generically.
- `ValueNodeBase`
- `ValueNode<E>`
  - represents enums or simple values. eg. type_specifier, storage_class_specifier, type_name
  - provides string representation via `getLabel()`
  - visitors may use typed `visitVal()` or erased `visit()` or both.

### 3.2. IR (LLVM-like)

We use a simple subset of LLVM IR, which can be emitted with `-l`.

- notably, no support for `phi`. Everything is `alloca`, `load`, `store`. We have no mem2reg pass.
- thogcc is designed to always _emit_ valid LLVM IR, but not take LLVM input.

### 3.3. Backend (RISCV)

- Target: `rv32gc`
- ABI: `ilp32d`

## 8. Development & Testing Environment

Local Setup Instructions: See [CONTRIBUTING.md](/CONTRIBUTING.md)

Testing: [lit](https://llvm.org/docs/CommandGuide/lit.html) test suites in [`tests/`](/tests/). Requires filecheck.

## 9. Future Considerations / Roadmap

## 11. Glossary / Acronyms

- SEMA: Semantic Analysis
- CRTP: [Curiously Recurring Template Pattern](https://en.cppreference.com/cpp/language/crtp)
- lit: [LLVM Integrated Tester](https://llvm.org/docs/CommandGuide/lit.html)
