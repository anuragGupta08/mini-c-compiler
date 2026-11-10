#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_FUNCTION,
    AST_RETURN,
    AST_UNARY,
    AST_PREPROCESSOR,
    AST_CONSTANT,
    AST_TYPE
} ASTNodeType;

typedef enum {
    OP_NEGATE,
    OP_COMPLEMENT
} UnaryOperator;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType kind;

    union {
        struct {
            char *name;
            ASTNode *return_type;
            ASTNode *body;
        } function;

        struct {
            char *directive;
        } preprocessor;

        struct {
            ASTNode *expr;
        } return_stmt;

        struct {
            int value;
        } constant;

        struct {
            char *name;
        } type;

        struct {
            UnaryOperator op;
            ASTNode *expr;
        } unary;
    };
};

ASTNode *parse(TokenArray *tokens);
void printAST(FILE *out, ASTNode *node, int depth);
void freeAST(ASTNode *node);

#endif