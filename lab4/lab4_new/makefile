CC		= gcc
CFLAGS		= -O3 -maltivec -Wall -pedantic -std=c11 -m64 -pthread -D_GNU_SOURCE
LDFLAGS		= -pthread -lpthread -lm
OBJS		= main.o list.o error.o random.o set.o dataflow.o timebase.o tbr.o sequential.o
OUT		= a.out

1: compile
	./a.out

10: compile
	a.out&&a.out&&a.out&&a.out&&a.out&&a.out&&a.out&&a.out&&a.out&&a.out
	
compile: $(OBJS)	
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(OUT)

tbr.o:
	$(CC) -m$(A) tbr.s -c

clean:
	rm -f *.o a.out score cfg.dot

cg: 1
	valgrind --tool=cachegrind --I1=65536,1,128 --D1=32768,2,128 --LL=1048576,8,128 ./a.out

hg: 1
	valgrind --tool=helgrind ./a.out
