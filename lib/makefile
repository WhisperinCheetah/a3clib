CFLAGS = -Wall -Wextra -Wno-missing-braces -Werror=pointer-arith -O3

NAME = a3clib
HEADER = $(NAME).h
OBJS = alg.o mem.o str.o das.o mth.o

all: $(NAME)

a3clib: $(HEADER) $(OBJS)
	ar rcs output/lib$(NAME).a $(OBJS)

alg.o: alg.c $(HEADER)
	gcc -c $< $(CFLAGS)

mem.o: mem.c $(HEADER)
	gcc -c $< $(CFLAGS)

str.o: str.c $(HEADER)
	gcc -c $< $(CFLAGS)

das.o: das.c $(HEADER)
	gcc -c $< $(CFLAGS)

mth.o: mth.c $(HEADER)
	gcc -c $< $(CFLAGS)

clean:
	rm -vf *.o output/lib$(NAME).a
