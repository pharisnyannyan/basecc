# 02_parser

Stub parser stage that wraps the lexer. This stage currently exposes a minimal
`Parser` API that forwards to the lexer so we can wire up the build, examples,
and tests before implementing full parsing.
