TARGET = polycalc

CC = gcc
CFLAGS = -g -Wall -Wextra

LEX_SRC = lex.yy.c
YACC_SRC = parser.tab.c
POLY_SRC = polynomial.c

OBJ = $(LEX_SRC:.c=.o) $(YACC_SRC:.c=.o) $(POLY_SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

clean:
	rm -f $(TARGET) $(OBJ) parser.tab.c parser.tab.h lex.yy.c

.PHONY: all clean
