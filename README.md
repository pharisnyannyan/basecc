# BaseCC

![CI](https://github.com/basecc/basecc/actions/workflows/ci.yml/badge.svg)

BaseCC is a minimal, BSD-style educational C compiler written in C. The primary goal is clarity and learnability. The compiler is designed to be read, understood, and extended step by step, with a long-term intent of becoming self-hosting.

This repository is being built as a challenge using ChatGPT Codex.

## Project goal
BaseCC is a **minimal, BSD-style educational C compiler written in C**. The primary goal is clarity and learnability. The compiler is designed to be read, understood, and extended step by step, with a long-term intent of becoming self-hosting.

The project deliberately favors:
- explicit control flow
- simple data structures
- boring, readable C
- minimal tooling

## Core principles
- BSD-leaning coding style: clear, disciplined, unsurprising C
- No clever tricks, no macro-heavy abstractions
- Each compiler stage is understandable in isolation
- Simplicity beats generality
- Educational value is more important than feature completeness

## Repository structure (stage-based)
The compiler is organized as **one directory per stage**. Each stage is responsible for a single transformation or concern.

General rules:
- Each stage lives in its own folder
- Each stage can be built and tested independently
- Stages are composed in order by higher-level drivers

Each stage directory should contain:
- `src/` — implementation
- `include/` — headers (if needed)
- `tests/` — unit tests for that stage
- `README.md` — short explanation of what the stage does and its inputs/outputs

## Testing requirements
- **Every stage must have unit tests**
- Tests must be small, explicit, and easy to inspect
- Tests should validate behavior, not implementation details

### Minimal testing only
Avoid complex or heavyweight frameworks.

Allowed:
- Minimal C test programs with simple `ASSERT`-style helpers
- Shell scripts that run binaries and compare output
- Small Python scripts (standard library only)

Tests should:
- be runnable locally
- fail loudly and clearly
- return non-zero exit codes on failure

## Test execution model
Testing should be **fully automated**.

Requirements:
- One command at the repository root runs **all tests**
- That same command is used by CI
- No manual per-stage test invocation required for normal use

Example approaches (choose one, keep it simple):
- `make test`
- `./test.sh`
- `python tools/run_tests.py`

The exact mechanism can evolve, but the interface should remain stable.

## Build approach (minimal by design)
The build system should stay intentionally simple.

Guidelines:
- Prefer plain `Makefile`s
- Avoid build generators and meta-build systems
- Each stage may have its own small `Makefile`
- A top-level `Makefile` may orchestrate stage builds once multiple stages exist

It is acceptable to:
- define build rules incrementally as stages are added
- keep early stages extremely simple (single binary, few objects)

Do not over-design the build system upfront.

## Continuous Integration
All tests must run in **GitHub Actions**.

CI requirements:
- Run on every push and pull request
- Build the project using the same commands documented locally
- Run the single top-level test command
- Fail on any warning-worthy errors or test failures

The README should include:
- a CI status badge
- a short note stating that CI runs the full test suite automatically

CI runs the full test suite automatically on every push and pull request.

## License
BaseCC is released under the MIT License. See `LICENSE` for details.
