CFLAGS = -Wall -Wextra -Wno-missing-braces -Werror=pointer-arith

OBJS = alg.o mem.o str.o

all: clib

clib: clib.h $(OBJS)
	ar rcs output/libclib.a $(OBJS)

alg.o: alg.c clib.h
	gcc -c $< $(CFLAGS)

mem.o: mem.c clib.h
	gcc -c $< $(CFLAGS)

str.o: str.c clib.h
	gcc -c $< $(CFLAGS)

clean:
	rm -vf *.o output/clib.a
