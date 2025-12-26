# Codegen unit tests

This directory holds the codegen unit tests and their fixtures.

## Test data layout

`testdata/` contains `.c` to `.ll` fixtures for unit tests. Each `.c` file is
paired with a matching `.ll` file that represents the expected LLVM IR output.

## Unit vs. integration tests

Unit tests cover code generation in isolation and should not require driver
files. If a test needs a driver (for example, to compile and run the generated
`.ll`), place all of the related files in `04_codegen/integration_tests/`
instead so the separation stays clear.
