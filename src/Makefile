all: main
CC = g++
CFLAGS = -g

OBJS1 = buffer.cpp common.cpp cpu.cpp interconnect.cpp test_cpu_icnt_ub.cpp
OBJS2 = buffer.cpp common.cpp cpu.cpp dram.cpp interconnect.cpp weightfetcher.cpp test_dram_icnt_wf.cpp

test1: buffer.cpp buffer.hpp common.cpp common.hpp cpu.cpp cpu.hpp interconnect.cpp interconnect.hpp test_cpu_icnt_ub.cpp
	$(CC) $(CFLAGS) -o test1 $(OBJS1)

test2: buffer.cpp buffer.hpp common.cpp common.hpp cpu.cpp cpu.hpp dram.cpp dram.hpp interconnect.cpp interconnect.hpp weightfetcher.cpp weightfetcher.hpp test_dram_icnt_wf.cpp
	$(CC) $(CFLAGS) -o test2 $(OBJS2)

clean:
	rm -f *~ *.o test1 test2
