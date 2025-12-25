# Integration tests

These tests exercise the full pipeline from lexing through code generation.

## Running locally

From the repository root:

```
make -C 04_codegen integration-test
```

You do not need to run `make test` first. The integration-test target builds
its required libraries and binaries before running the driver.

The `.ll` compile step uses `LL_CC`, which defaults to `CC`. If your default
compiler cannot consume LLVM IR, run with a clang-based compiler:

```
make -C 04_codegen integration-test LL_CC=clang
```

## CI

These tests run automatically on every push and pull request.
