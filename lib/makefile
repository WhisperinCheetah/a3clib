CFLAGS = -Wall -Wextra -Wno-missing-braces -Werror=pointer-arith

OBJS = alg.o mem.o

all: clib

clib: clib.h $(OBJS)
	ar rcs output/libclib.a $(OBJS)

alg.o: alg.c clib.h mem.o
	gcc -c $< $(CFLAGS)

mem.o: mem.c clib.h
	gcc -c $< $(CFLAGS)

clean:
	rm -vf *.o output/clib.a
