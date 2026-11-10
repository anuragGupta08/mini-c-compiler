#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "tokens.h"

TokenArray *lex(const char *src, FILE *out);

#endif