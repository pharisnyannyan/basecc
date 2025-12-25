# Lexer stage

This stage turns an input byte stream into a stream of tokens. The lexer is
minimal by design and focuses on clarity over completeness.

## Supported tokens
- identifiers: `[A-Za-z_][A-Za-z0-9_]*`
- numbers: decimal integers
- punctuators: `+ - * / ( ) { } ; , == != =`
- keywords: `if else while switch case int`
- invalid: any unsupported single character
- end-of-file marker

Whitespace is skipped and never emitted as a token.

## Build
From the repository root:
```
make -C 01_lexer
```

## Test
From the repository root:
```
make test
```
