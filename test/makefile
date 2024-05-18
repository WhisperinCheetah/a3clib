CFLAGS = -Wall -Wextra -Wno-missing-braces -Werror=pointer-arith

all: test.o
	gcc $(CFLAGS) -I../lib/ -L../lib/output/ -o main test.o -lclib

test.o: test.c
	gcc $(CFLAGS) -I../lib/ -L../lib/output/ -c test.c
