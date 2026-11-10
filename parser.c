#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

static TokenArray *tokens;
static int current = 0;

/* Parser function declarations */

static ASTNode *parseFunction(void);
static ASTNode *parseExpression(void);
static ASTNode *parseReturn(void);
static ASTNode *parseUnary(void);
static ASTNode *parseConstant(void);
static ASTNode *parseType(void);
static ASTNode *parsePreprocessor(void);

Token *peek(){
    return &tokens->tokens[current];
}

Token *advance(){
    return &tokens->tokens[current++];
}

ASTNode *parse(TokenArray *input)
{
    tokens = input;
    current = 0;

    /* Skip preprocessor directives for now */
    while (peek()->type == TOKEN_PREPROCESSOR)
    {
        ASTNode *pp = parsePreprocessor();
        freeAST(pp);
    }
    ASTNode *root = parseFunction();
    return root;
}

void expect(TokenType expected)
{
    if (peek()->type == expected)
    {
        advance();
    }
    else
    {
        printf("Syntax Error\n");
        printf("Expected token type: %d\n", expected);
        printf("Found token type: %d\n", peek()->type);
        exit(1);
    }
}

ASTNode *newNode(ASTNodeType type){
    ASTNode *node = malloc(sizeof(ASTNode));
    if(node==NULL){
        printf("Memory allocation failed.\n");
        exit(1);
    } 
    node->kind=type;
    return node;
}

ASTNode *parsePreprocessor()
{
    if (peek()->type != TOKEN_PREPROCESSOR)
    {
        printf("Expected preprocessor directive\n");
        exit(1);
    }

    ASTNode *node = newNode(AST_PREPROCESSOR);

    node->preprocessor.directive = strdup(peek()->value);

    advance();

    return node;
}

ASTNode *parseType(){
    if(peek()->type == TOKEN_KEYWORD && strcmp(peek()->value,"int")==0){
        ASTNode *type = newNode(AST_TYPE);
        type->type.name = strdup("int");
        advance();
        return type;
    }
    printf("Expected Type\n");
    exit(1);
}

ASTNode *parseConstant() {
    
    if (peek()->type != TOKEN_CONSTANT)
    {
        printf("Expected constant\n");
        exit(1);
    }
    ASTNode *node = newNode(AST_CONSTANT);

    node->constant.value = atoi(peek()->value);

    advance();

    return node;
}

ASTNode *parseUnary(){

    ASTNode *node = newNode(AST_UNARY);
    if (peek()->type == TOKEN_HYPHEN)
    {
    node->unary.op = OP_NEGATE;
    advance();
    }
    else if (peek()->type == TOKEN_TILDE)
    {
        node->unary.op = OP_COMPLEMENT;
        advance();
    }
    else
    {
        printf("Expected unary operator\n");
        exit(1);
    }

    node->unary.expr = parseExpression();
    return node;
}

ASTNode *parseExpression()
{
    if (peek()->type == TOKEN_CONSTANT)
    {
        return parseConstant();
    }

    if (peek()->type == TOKEN_HYPHEN ||
        peek()->type == TOKEN_TILDE)
    {
        return parseUnary();
    }

    if (peek()->type == TOKEN_OPEN_PAREN)
    {
        advance();

        ASTNode *expr = parseExpression();

        expect(TOKEN_CLOSE_PAREN);

        return expr;
    }

    printf("Expected expression\n");
    exit(1);
}

ASTNode *parseReturn()
{
    if (peek()->type != TOKEN_KEYWORD ||
        strcmp(peek()->value, "return") != 0)
    {
        printf("Expected return\n");
        exit(1);
    }

    advance();
    ASTNode *expr = parseExpression();
    expect(TOKEN_SEMICOLON);

    ASTNode *ret = newNode(AST_RETURN);
    ret->return_stmt.expr = expr;

    return ret;
}

ASTNode *parseFunction()
{
    ASTNode *func = newNode(AST_FUNCTION);

    func->function.return_type = parseType();

    if (peek()->type != TOKEN_IDENTIFIER)
    {
        printf("Expected function name\n");
        exit(1);
    }

    func->function.name = strdup(peek()->value);
    advance();

    expect(TOKEN_OPEN_PAREN);

    if (peek()->type != TOKEN_KEYWORD ||
        strcmp(peek()->value, "void") != 0)
    {
        printf("Expected void\n");
        exit(1);
    }

    advance();

    expect(TOKEN_CLOSE_PAREN);
    expect(TOKEN_OPEN_BRACE);

    func->function.body = parseReturn();

    expect(TOKEN_CLOSE_BRACE);
    expect(TOKEN_EOF);

    return func;
}

void printAST(FILE *out, ASTNode *node, int depth)
{
    if (node == NULL)
        return;

    for (int i = 0; i < depth; i++)
        fprintf(out, "    ");

    switch (node->kind)
    {
        case AST_FUNCTION:
            fprintf(out, "Function: %s\n", node->function.name);
            printAST(out, node->function.return_type, depth + 1);
            printAST(out, node->function.body, depth + 1);
            break;

        case AST_TYPE:
            fprintf(out, "Type: %s\n", node->type.name);
            break;

        case AST_RETURN:
            fprintf(out, "Return\n");
            printAST(out, node->return_stmt.expr, depth + 1);
            break;

        case AST_CONSTANT:
            fprintf(out, "Constant: %d\n", node->constant.value);
            break;

        case AST_PREPROCESSOR:
            fprintf(out, "Preprocessor: %s\n",
                    node->preprocessor.directive);
            break;
        case AST_UNARY:
            if (node->unary.op == OP_NEGATE)
                fprintf(out, "Unary: Negate\n");
            else if (node->unary.op == OP_COMPLEMENT)
                fprintf(out, "Unary: Complement\n");
            printAST(out, node->unary.expr, depth + 1);
            break;
    }
}

void freeAST(ASTNode *node)
{
    if (node == NULL)
        return;

    switch (node->kind)
    {
        case AST_FUNCTION:
            free(node->function.name);
            freeAST(node->function.return_type);
            freeAST(node->function.body);
            break;

        case AST_RETURN:
            freeAST(node->return_stmt.expr);
            break;

        case AST_UNARY:
            freeAST(node->unary.expr);
            break;

        case AST_PREPROCESSOR:
            free(node->preprocessor.directive);
            break;

        case AST_CONSTANT:
            break;

        case AST_TYPE:
            free(node->type.name);
            break;
    }

    free(node);
}