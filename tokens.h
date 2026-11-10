#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_PREPROCESSOR,

    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_CONSTANT,

    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSE_BRACE,
    TOKEN_SEMICOLON,

    TOKEN_HYPHEN,
    TOKEN_DECREMENT,
  
    TOKEN_TILDE,
    TOKEN_NOT,
    TOKEN_XOR,
    TOKEN_AND,

    TOKEN_EOF
} TokenType;


typedef struct {
    TokenType type;
    char *value;
} Token;


typedef struct {
    Token *tokens;
    int count;
} TokenArray;

#endif