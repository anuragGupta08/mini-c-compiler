#ifndef TACKY_H
#define TACKY_H

#include "parser.h"

typedef enum{
    TACKY_RETURN,
    TACKY_UNARY
}TackyInstructionType;

typedef enum{
    TACKY_NEGATE,
    TACKY_COMPLEMENT
}TackyUnaryOperator;

typedef struct TackyValue{
    int constant;
    int is_temp;
    char *temp_name;
}TackyValue;

typedef struct TackyInstruction TackyInstruction;

struct TackyInstruction {
    TackyInstructionType type;

    union {
        struct {
            TackyUnaryOperator op;
            TackyValue src;
            TackyValue dst;
        } unary;

        struct {
            TackyValue value;
        } return_instr;
    };
};

typedef struct {
    char *name;

    TackyInstruction *instructions;
    int count;
    int capacity;
} TackyFunction;

typedef struct {
    TackyFunction function;
} TackyProgram;

TackyProgram *generateTacky(ASTNode *ast);

void printTacky(FILE *out, TackyProgram *program);

void freeTacky(TackyProgram *program);

#endif