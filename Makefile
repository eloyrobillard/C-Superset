# NOTE -g Produce debugging information in the operating system's native format (stabs, COFF, XCOFF,
#            or DWARF).  GDB can work with this debugging information.
CFLAGS=-std=c11 -g -static

# gcc -o 9cc 9cc.c が自動的に行われる
9cc:

test: 9cc
				./test.sh


clean:
				rm -f 9cc *.o *~ tmp*

# NOTE -pg Generate extra code to write profile information suitable for the analysis program prof (for
#            -p) or gprof (for -pg).  You must use this option when compiling the source files you want
#            data about, and you must also use it when linking.
profout:
				gcc -pg 9cc.c
				./a.out " 5 +6 -4 +20 + 5 "

profile: profout
				./a.out " 5 +6 -4 +20 + 5 "
				gprof ./a.out gmon.out

memcheck: profout
				valgrind --leak-check=yes ./a.out " 5 +6 -4 +20 + 5 "

cleanp:
				rm -f *.out

.MEM: memcheck cleanp
.PHONY: test clean
.PERF: profile cleanp