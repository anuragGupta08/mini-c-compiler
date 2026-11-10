# Mini C Compiler

A small C compiler written from scratch in C. It compiles a subset of C
to x86-64 assembly (AT&T syntax), built stage by stage to learn how
compilers work.

## Pipeline

```
Source → Lexer → Parser (AST) → TACKY IR → Assembly AST → Code Emitter → program.s
```

| Stage | File | Description |
|---|---|---|
| Lexer | `lexer.c` | Converts source text into tokens |
| Parser | `parser.c` | Recursive-descent parser that builds an AST |
| IR generation | `tacky.c` | Lowers the AST to three-address code (TACKY) |
| Assembly generation | `assembly.c` | Converts TACKY into an assembly-level tree |
| Emitter | `emitter.c` | Writes x86-64 assembly text |

Each stage writes a readable debug dump (`tokens.txt`, `ast.txt`,
`tacky.txt`, `assembly.txt`) so you can inspect the compiler's work.

## Supported language features

- `int` functions with `void` parameters
- A single `return` statement
- Integer constants
- Unary operators: `-` (negate) and `~` (bitwise complement)
- Parenthesized expressions
- Preprocessor lines (e.g. `#include`) are lexed and skipped

## Build and run

```bash
make
./compiler test.c
gcc program.s -o program
./program
echo $?     # prints the return value
```

## Example

Input (`test.c`):
```c
int main(void) {
    return 2;
}
```

Generated `program.s`:
```asm
.globl main
main:
    movl $2, %eax
    ret

.section .note.GNU-stack,"",@progbits
```

For `return -(~2);`, the TACKY intermediate representation (`tacky.txt`) is:
```
Unary(Complement, Constant(2), Var(temp.0))
Unary(Negate, Var(temp.0), Var(temp.1))
Return(Var(temp.1))
```

## Known limitations / roadmap

- Only one `return` statement per function
- Unary expressions: the front end (lexer, parser, TACKY) is complete, but
  the backend still needs proper stack allocation. Every temporary currently
  maps to the same stack slot, and the emitter does not yet set up a stack
  frame (`pushq %rbp` / `movq %rsp, %rbp`)
- Planned: fix the above, then binary operators, variables, `if`/`else`,
  loops, function calls

## What I learned

Building this compiler taught me how a program is transformed in stages: the
lexer turns raw text into tokens, a recursive-descent parser turns tokens into
an AST, and each later stage only has to understand the output of the one
before it. Adding the TACKY intermediate representation was the biggest
lesson. Flattening nested expressions like `-(~2)` into simple three-address
instructions with temporaries made code generation much easier than working
directly from the tree. The hardest part was the backend: translating
temporaries into assembly operands and stack slots showed me how much a
compiler has to manage (such as stack frames and memory-to-memory moves) that
is invisible when writing C. I know my current stack allocation is incomplete,
and fixing it is my next step before adding binary operators and variables.