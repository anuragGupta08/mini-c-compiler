CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = main.c lexer.c parser.c tacky.c assembly.c emitter.c

compiler: $(SRC)
	$(CC) $(CFLAGS) -o compiler $(SRC)

clean:
	rm -f compiler tokens.txt ast.txt tacky.txt assembly.txt program.s