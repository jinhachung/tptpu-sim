all: main
CC = g++
CFLAGS = -g

OBJS1 = buffer.cpp common.cpp cpu.cpp interconnect.cpp test_cpu_icnt_ub.cpp
OBJS2 = buffer.cpp common.cpp cpu.cpp dram.cpp interconnect.cpp weightfetcher.cpp test_dram_icnt_wf.cpp
OBJS3 = buffer.cpp common.cpp cpu.cpp dram.cpp interconnect.cpp mmu.cpp weightfetcher.cpp test_mmu.cpp

test1: buffer.cpp buffer.hpp common.cpp common.hpp cpu.cpp cpu.hpp interconnect.cpp interconnect.hpp unit.hpp test_cpu_icnt_ub.cpp
	$(CC) $(CFLAGS) -o test1 $(OBJS1)

test2: buffer.cpp buffer.hpp common.cpp common.hpp cpu.cpp cpu.hpp dram.cpp dram.hpp interconnect.cpp interconnect.hpp unit.hpp weightfetcher.cpp weightfetcher.hpp test_dram_icnt_wf.cpp
	$(CC) $(CFLAGS) -o test2 $(OBJS2)

test3: buffer.cpp buffer.hpp common.cpp common.hpp cpu.cpp cpu.hpp dram.cpp dram.hpp interconnect.cpp interconnect.hpp mmu.cpp mmu.hpp unit.hpp weightfetcher.cpp weightfetcher.hpp test_mmu.cpp
	$(CC) $(CFLAGS) -o test3 $(OBJS3)

clean:
	rm -f *~ *.o test1 test2 test3
