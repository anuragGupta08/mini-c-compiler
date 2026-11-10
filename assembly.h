#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "tacky.h"

typedef enum
{
    ASM_PROGRAM,
    ASM_FUNCTION,

    ASM_MOV,
    ASM_NEG,
    ASM_NOT,
    ASM_RET,

    ASM_IMMEDIATE,
    ASM_REGISTER,
    ASM_STACK
} ASMNodeType;

typedef struct ASMNode ASMNode;

struct ASMNode
{
    ASMNodeType type;

    union
    {
        struct
        {
            ASMNode *function;
        } program;

        struct
        {
            char *name;
            ASMNode *instructions[20];
            int instruction_count;
        } function;

        struct
        {
            ASMNode *src;
            ASMNode *dst;
        } mov;

        struct
        {
            ASMNode *operand;
        } unary;

        struct
        {
            int value;
        } imm;

        struct
        {
            char *name;
        } reg;

        struct
        {
            int offset;
        } stack;
    };
};

ASMNode *generateAssembly(TackyProgram *tacky);

void printAssembly(FILE *out, ASMNode *program);

void freeAssembly(ASMNode *node);

#endif