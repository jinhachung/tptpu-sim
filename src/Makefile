all: main
CC = g++
CFLAGS = -g

OBJS1 = buffer.cpp common.cpp cpu.cpp interconnect.cpp test_cpu_icnt_ub.cpp

test1: buffer.cpp buffer.hpp common.cpp common.hpp cpu.cpp cpu.hpp interconnect.cpp interconnect.hpp test_cpu_icnt_ub.cpp
	$(CC) $(CFLAGS) -o test1 $(OBJS1)

clean:
	rm -f *~ *.o test1
