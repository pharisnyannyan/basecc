# Lexer stage

This stage turns an input byte stream into a stream of tokens. The lexer is designed to be simple and robust.

## Supported Tokens
- **Identifiers**: `[A-Za-z_][A-Za-z0-9_]*`
- **Numbers**: Decimal integers.
- **Punctuators**: `+`, `-`, `*`, `/`, `(`, `)`, `{`, `}`, `;`, `,`, `==`, `!=`, `=`, `<`, `>`, `<=`, `>=`, `&`, `!`, `.`, `->`, `[`, `]`.
- **Keywords**: `if`, `else`, `while`, `for`, `return`, `break`, `continue`, `int`, `char`, `struct`, `typedef`, `sizeof`, `extern`, `static`, `const`.
- **Invalid**: Any unsupported character is emitted as an invalid token for error reporting.
- **EOF**: End-of-file marker.

## Behavior
- Whitespace and comments (standard C style) are skipped.
- Tokens include their location (line, column) for error reporting.

## Build and Test
Run `make all` and `make test` from the repository root to build and verify the lexer.
