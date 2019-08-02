# Makefile to take care of both Makefile.tptpu1 and Makefile.tptpu2

tptpu1:
	make tptpu1 -f Makefile.tptpu1

sim1:
	make tptpu1-sim -f Makefile.tptpu1

mmu1:
	make tptpu1-mmutest -f Makefile.tptpu1

tile1:
	make tptpu1-tiletest -f Makefile.tptpu1

tptpu2:
	make tptpu2 -f Makefile.tptpu2

clean:
	-make clean -f Makefile.tptpu1
	-make clean -f Makefile.tptpu2
