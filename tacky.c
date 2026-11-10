#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tacky.h"

static int tempCount=0;

static TackyValue makeConstant(int value){
    TackyValue v;
    v.constant=value;
    v.is_temp=0;
    v.temp_name=NULL;
    return v;   
}

static TackyValue makeTemp(void){
    TackyValue v;
    v.constant=0;
    v.is_temp=1;
    char name[50];
    sprintf(name,"temp.%d",tempCount++);
    v.temp_name = strdup(name);
    return v;
}

static void addInstruction(
    TackyFunction *function,
    TackyInstruction instruction)
{
    if (function->count >= function->capacity)
    {
        function->capacity *= 2;

        function->instructions =
            realloc(
                function->instructions,
                function->capacity *
                sizeof(TackyInstruction)
            );

        if (function->instructions == NULL)
        {
            perror("realloc");  
            exit(1);
        }
    }

    function->instructions[function->count++] = instruction;
}

static TackyValue emitExpression(
    ASTNode *node,
    TackyFunction *function)
{
    /* Constant */

    if (node->kind == AST_CONSTANT)
    {
        return makeConstant(node->constant.value);
    }

    /* Unary expression */

    if (node->kind == AST_UNARY)
    {
        TackyValue src =
            emitExpression(node->unary.expr, function);

        TackyValue dst = makeTemp();

        TackyInstruction instruction;

        instruction.type = TACKY_UNARY;

        if (node->unary.op == OP_NEGATE)
        {
            instruction.unary.op = TACKY_NEGATE;
        }
        else
        {
            instruction.unary.op = TACKY_COMPLEMENT;
        }

        instruction.unary.src = src;
        instruction.unary.dst = dst;

        addInstruction(function, instruction);

        return dst;
    }

    fprintf(stderr, "Unsupported expression\n");
    exit(1);
}

static void emitReturn(
    ASTNode *node,
    TackyFunction *function)
{
    TackyValue value =
        emitExpression(
            node->return_stmt.expr,
            function
        );

    TackyInstruction instruction;

    instruction.type = TACKY_RETURN;

    instruction.return_instr.value = value;

    addInstruction(function, instruction);
}

TackyProgram *generateTacky(ASTNode *ast)
{
    TackyProgram *program =
        malloc(sizeof(TackyProgram));

    if (program == NULL)
    {
        perror("malloc");
        exit(1);
    }

    program->function.name =
        strdup(ast->function.name);

    program->function.count = 0;
    program->function.capacity = 8;

    program->function.instructions =
        malloc(
            program->function.capacity *
            sizeof(TackyInstruction)
        );

    if (program->function.instructions == NULL)
    {
        perror("malloc");
        exit(1);
    }

    emitReturn(
        ast->function.body,
        &program->function
    );

    return program;
}

void printTacky(FILE *out, TackyProgram *program)
{
    fprintf(out, "Program\n");
    fprintf(out, "------------\n");

    fprintf(
        out,
        "Function: %s\n",
        program->function.name
    );

    for (int i = 0;
         i < program->function.count;
         i++)
    {
        TackyInstruction *instruction =
            &program->function.instructions[i];

        fprintf(out, "    ");

        if (instruction->type == TACKY_UNARY)
        {
            const char *op;

            if (instruction->unary.op == TACKY_NEGATE)
                op = "Negate";
            else
                op = "Complement";

            if (instruction->unary.src.is_temp)
            {
                fprintf(
                    out,
                    "Unary(%s, Var(%s), Var(%s))\n",
                    op,
                    instruction->unary.src.temp_name,
                    instruction->unary.dst.temp_name
                );
            }
            else
            {
                fprintf(
                    out,
                    "Unary(%s, Constant(%d), Var(%s))\n",
                    op,
                    instruction->unary.src.constant,
                    instruction->unary.dst.temp_name
                );
            }
        }
        else if (instruction->type == TACKY_RETURN)
        {
            if (instruction->return_instr.value.is_temp)
            {
                fprintf(
                    out,
                    "Return(Var(%s))\n",
                    instruction->return_instr.value.temp_name
                );
            }
            else
            {
                fprintf(
                    out,
                    "Return(Constant(%d))\n",
                    instruction->return_instr.value.constant
                );
            }
        }
    }
}

void freeTacky(TackyProgram *program)
{
    if (program == NULL)
        return;

    free(program->function.name);

    for (int i = 0; i < program->function.count; i++)
    {
        TackyInstruction *instruction =
            &program->function.instructions[i];

        if (instruction->type == TACKY_UNARY)
        {
            if (instruction->unary.dst.is_temp)
                free(instruction->unary.dst.temp_name);
        }
    }

    free(program->function.instructions);
    free(program);
}