#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembly.h"

static ASMNode *newNode(ASMNodeType type)
{
    ASMNode *node = malloc(sizeof(ASMNode));

    if (node == NULL)
    {
        perror("malloc");
        exit(1);
    }

    node->type = type;
    return node;
}

static void addInstruction(ASMNode *function, ASMNode *instruction)
{
    if (function->function.instruction_count >= 20)
    {
        fprintf(stderr, "Too many assembly instructions\n");
        exit(1);
    }

    function->function.instructions[
        function->function.instruction_count++
    ] = instruction;
}

static ASMNode *makeOperand(TackyValue value)
{
    if (!value.is_temp)
    {
        ASMNode *node = newNode(ASM_IMMEDIATE);
        node->imm.value = value.constant;
        return node;
    }

    ASMNode *node = newNode(ASM_STACK);
    node->stack.offset = -4;
    return node;
}

ASMNode *generateAssembly(TackyProgram *tacky)
{
    if (tacky == NULL)
        return NULL;

    ASMNode *program = newNode(ASM_PROGRAM);

    ASMNode *function = newNode(ASM_FUNCTION);
    function->function.name = strdup(tacky->function.name);
    function->function.instruction_count = 0;

    program->program.function = function;

    for (int i = 0; i < tacky->function.count; i++)
    {
        TackyInstruction *instr =
            &tacky->function.instructions[i];

        if (instr->type == TACKY_UNARY)
        {
            /* Source */
            ASMNode *src = makeOperand(instr->unary.src);

            /* Destination for MOV */
            ASMNode *dst = newNode(ASM_STACK);
            dst->stack.offset = -4;

            ASMNode *mov = newNode(ASM_MOV);
            mov->mov.src = src;
            mov->mov.dst = dst;

            addInstruction(function, mov);

            /* Unary instruction */
            ASMNode *unary;

            if (instr->unary.op == TACKY_NEGATE)
                unary = newNode(ASM_NEG);
            else
                unary = newNode(ASM_NOT);
                
            ASMNode *unaryOperand = newNode(ASM_STACK);
            unaryOperand->stack.offset = -4;

            unary->unary.operand = unaryOperand;

            addInstruction(function, unary);
        }
        else if (instr->type == TACKY_RETURN)
        {
            ASMNode *src =
                makeOperand(instr->return_instr.value);

            ASMNode *eax = newNode(ASM_REGISTER);
            eax->reg.name = strdup("EAX");

            ASMNode *mov = newNode(ASM_MOV);
            mov->mov.src = src;
            mov->mov.dst = eax;

            addInstruction(function, mov);

            ASMNode *ret = newNode(ASM_RET);
            addInstruction(function, ret);
        }
    }

    return program;
}

static void printOperand(FILE *out, ASMNode *node)
{
    switch (node->type)
    {
        case ASM_IMMEDIATE:
            fprintf(out, "        Imm(%d)\n", node->imm.value);
            break;

        case ASM_REGISTER:
            fprintf(out, "        Register(%s)\n", node->reg.name);
            break;

        case ASM_STACK:
            fprintf(out, "        Stack(%d)\n", node->stack.offset);
            break;

        default:
            break;
    }
}

static void printInstruction(FILE *out, ASMNode *node)
{
    switch (node->type)
    {
        case ASM_MOV:
            fprintf(out, "    Mov\n");
            printOperand(out, node->mov.src);
            printOperand(out, node->mov.dst);
            fprintf(out, "\n");
            break;

        case ASM_NEG:
            fprintf(out, "    Neg\n");
            printOperand(out, node->unary.operand);
            fprintf(out, "\n");
            break;

        case ASM_NOT:
            fprintf(out, "    Not\n");
            printOperand(out, node->unary.operand);
            fprintf(out, "\n");
            break;

        case ASM_RET:
            fprintf(out, "    Ret\n\n");
            break;

        default:
            break;
    }
}

void printAssembly(FILE *out, ASMNode *program)
{
    if (program == NULL)
        return;

    fprintf(out, "Program\n");
    fprintf(out, "------------\n");

    ASMNode *function = program->program.function;

    fprintf(out, "Function: %s\n",
            function->function.name);

    for (int i = 0;
         i < function->function.instruction_count;
         i++)
    {
        printInstruction(
            out,
            function->function.instructions[i]
        );
    }
}

void freeAssembly(ASMNode *node)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
        case ASM_PROGRAM:
            freeAssembly(node->program.function);
            break;

        case ASM_FUNCTION:
            free(node->function.name);

            for (int i = 0;
                 i < node->function.instruction_count;
                 i++)
            {
                freeAssembly(
                    node->function.instructions[i]
                );
            }
            break;

        case ASM_MOV:
            freeAssembly(node->mov.src);
            freeAssembly(node->mov.dst);
            break;

        case ASM_NEG:
        case ASM_NOT:
            freeAssembly(node->unary.operand);
            break;

        case ASM_REGISTER:
            free(node->reg.name);
            break;

        case ASM_IMMEDIATE:
        case ASM_STACK:
        case ASM_RET:
            break;
    }

    free(node);
}