CFLAGS=-std=c11 -g -static

# gcc -o 9cc 9cc.c が自動的に行われる
9cc:

test: 9cc
				./test.sh

clean:
				rm -f 9cc *.o *~ tmp*

.PHONY: test clean