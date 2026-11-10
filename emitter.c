#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emitter.h"

static void emitOperand(FILE *out, ASMNode *node)
{
    switch (node->type)
    {
        case ASM_IMMEDIATE:
            fprintf(out, "$%d", node->imm.value);
            break;

        case ASM_STACK:
            fprintf(out, "%d(%%rbp)", node->stack.offset);
            break;

        case ASM_REGISTER:
            if (strcmp(node->reg.name, "EAX") == 0)
                fprintf(out, "%%eax");
            break;

        default:
            break;
    }
}

static void emitInstruction(FILE *out, ASMNode *node)
{
    switch (node->type)
    {
        case ASM_MOV:
            fprintf(out, "    movl ");
            emitOperand(out, node->mov.src);
            fprintf(out, ", ");
            emitOperand(out, node->mov.dst);
            fprintf(out, "\n");
            break;

        case ASM_NEG:
            fprintf(out, "    negl ");
            emitOperand(out, node->unary.operand);
            fprintf(out, "\n");
            break;

        case ASM_NOT:
            fprintf(out, "    notl ");
            emitOperand(out, node->unary.operand);
            fprintf(out, "\n");
            break;

        case ASM_RET:
            fprintf(out, "    ret\n");
            break;

        default:
            break;
    }
}

void emitAssembly(FILE *out, ASMNode *program)
{
    if (program == NULL)
        return;

    ASMNode *function = program->program.function;

    fprintf(out, ".globl %s\n", function->function.name);
    fprintf(out, "%s:\n", function->function.name);

    for (int i = 0;
         i < function->function.instruction_count;
         i++)
    {
        emitInstruction(
            out,
            function->function.instructions[i]
        );
    }

    fprintf(out,
            "\n.section .note.GNU-stack,\"\",@progbits\n");
}