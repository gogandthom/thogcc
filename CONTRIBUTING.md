# Contributing

## Building

### Making project in VSCode

1. Run `thogcc: cmake configure` task
2. Run `thogcc: build` task (or ctrl-shift-B)
3. Binary in `build/c_compiler`
4. Yay

### Making project on command line

1. `make` or `make build/c_compiler`
2. Yay
3. If using the clangd extension, this may subsequently break syntax highlighting things in VSCode.
4. To unfuck language server, ctrl-shift-P and `CMake: Delete Cache and Reconfigure`.

## Design

- Bison/Flex parses C89.
- Internally we use a simple subset of [LLVM IR](https://llvm.org/docs/LangRef.html) (can be emitted with `-l` flag)
  - notably, no support for `phi`. Everything is `alloca`, `load`, `store`. We have no mem2reg pass.
  - thogcc is designed to always _emit_ valid LLVM IR, but not take LLVM input.
- Backend: RISCV, target: `rv32gc`, ABI: `ilp32d`.

For more details see [ARCHITECTURE.md](/ARCHITECTURE.md).
