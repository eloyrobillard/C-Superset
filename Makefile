# NOTE -g Produce debugging information in the operating system's native format (stabs, COFF, XCOFF,
#            or DWARF).  GDB can work with this debugging information.
CFLAGS=-std=c11 -g -static

# NOTE gcc $(CFLAGS) -o 9cc 9cc.c が自動的に行われる
9cc:

test: 9cc
				./test.sh

clean:
				rm -f 9cc *.o *~ tmp*

# NOTE -pg Generate extra code to write profile information suitable for gprof. You must use this option when compiling
# 				 the source files you want data about, and you must also use it when linking.
profout:
				gcc -pg 9cc.c

profile: profout
				./a.out " 5 +6 -4 +20 + 5 "
				gprof ./a.out gmon.out

memcheck: profout
				valgrind --leak-check=yes ./a.out " 5 +6 -4 +20 + 5 "

cleanp:
				rm -f *.out

.PHONY: test clean
.MEM: memcheck cleanp
.PERF: profile cleanp

math-rd:
test-math: math-rd
						./test.sh
clean-math:
				rm -f math-rd *.o *~ tmp*
.MATH: test-math clean-math