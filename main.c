#include<stdio.h>
#include<stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "tacky.h"
#include "assembly.h"
#include "emitter.h"

int main(int argc, char *argv[]){

    if(argc != 2)
    { 
        printf("Usage: %s <source-file>\n", argv[0]); 
        return 1; 
    }

    /* Open source file */
    FILE *fp = fopen(argv[1],"r");
    if(fp == NULL){
        perror("fopen");
        return 1;
    }

    /* Find size of source file */ 
    fseek(fp, 0, SEEK_END); 
    long size = ftell(fp); 
    rewind(fp);

    /* Allocate memory for source code */
    char *program = malloc(size + 1);
    if(program == NULL){
        perror("malloc");
        fclose(fp);
        return 1;
    }

    /* Read source code into memory */
    fread(program, 1, size, fp);
    program[size] = '\0';

    fclose(fp);

    /* Open debug output */
    FILE *out = fopen("tokens.txt", "w");
    if (out == NULL){
        perror("tokens.txt");
        free(program);
        return 1;
    }

    /* Lexer stage */
    TokenArray *tokens = lex(program,out);
    if(tokens == NULL){
        fprintf(stderr,"Lexing failed\n");
        fclose(out);
        free(program);
        return 1;
    }

    fclose(out);

    printf("Lexing successfull\n");

    /* Parsing stage */

    ASTNode *ast = parse(tokens);
    
    if(ast==NULL){
        fprintf(stderr, "Parsing failed\n");
        free(program);
        return 1;
    }

    printf("Parsing successful.\n");

    /*Printing AST*/

    FILE *ast_out = fopen("ast.txt", "w");
    if (ast_out == NULL)
    {
        perror("ast.txt");

        freeAST(ast);
        free(program);

        return 1;
    }

    fprintf(ast_out, "AST\n");
    fprintf(ast_out, "------------\n");

    printAST(ast_out, ast, 0);

    fclose(ast_out);

    printf("AST written to ast.txt\n");

    /* Tacky IR generation */

    TackyProgram *tacky = generateTacky(ast);

    if (tacky == NULL)
    {
        fprintf(stderr, "Tacky generation failed\n");
        freeAST(ast);
        return 1;
    }

    printf("Tacky generation successful.\n");

    FILE *tacky_out = fopen("tacky.txt", "w");

    if (tacky_out == NULL)
    {
        perror("tacky.txt");
        freeTacky(tacky);
        freeAST(ast);
        return 1;
    }

    printTacky(tacky_out, tacky);

    fclose(tacky_out);

    printf("Tacky written to tacky.txt\n");


    /* ================= Assembly ================= */

    ASMNode *assembly = generateAssembly(tacky);

    if (assembly == NULL)
    {
        fprintf(stderr, "Assembly generation failed\n");

        freeTacky(tacky);
        freeAST(ast);

        for (int i = 0; i < tokens->count; i++)
            free(tokens->tokens[i].value);

        free(tokens->tokens);
        free(tokens);
        free(program);

        return 1;
    }

    printf("Assembly generation successful.\n");

    /* Print Assembly */

    FILE *assembly_out = fopen("assembly.txt", "w");

    if (assembly_out == NULL)
    {
        perror("assembly.txt");

        freeAssembly(assembly);
        freeTacky(tacky);
        freeAST(ast);

        for (int i = 0; i < tokens->count; i++)
            free(tokens->tokens[i].value);

        free(tokens->tokens);
        free(tokens);
        free(program);

        return 1;
    }

    printAssembly(assembly_out, assembly);

    fclose(assembly_out);

    printf("Assembly written to assembly.txt\n");

    /* ================= Emitter ================= */

    FILE *output = fopen("program.s", "w");

    if (output == NULL)
    {
        perror("program.s");

        freeAssembly(assembly);
        freeTacky(tacky);
        freeAST(ast);
        free(program);

        return 1;
    }

    emitAssembly(output, assembly);

    fclose(output);

    printf("Machine assembly written to program.s\n");

    /* ================= Cleanup ================= */

    freeAssembly(assembly);
    freeTacky(tacky);
    freeAST(ast);

    for (int i = 0; i < tokens->count; i++)
        free(tokens->tokens[i].value);

    free(tokens->tokens);
    free(tokens);

    free(program);

    printf("Compiler pipeline completed successfully.\n");

    return 0;
}
