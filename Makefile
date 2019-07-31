TESTTILE := ./build/testtile.exe
TESTMMU := ./build/testmmu.exe

CC := g++
CPP_SUFFIX := cpp

INCLUDE_DIR := -I./include/tpu_uarch
SRC_DIR = ./src
OBJ_DIR = ./obj
BUILD_DIR = ./build

CFLAGS := -g -Wall -std=c++11
LDFLAGS :=
LIBS :=

# all sources
SRC = $(wildcard $(SRC_DIR)/*.$(CPP_SUFFIX))
SRC += $(wildcard $(SRC_DIR)/**/*.$(CPP_SUFFIX))

# objects
OBJ = $(patsubst $(SRC_DIR)/%.$(CPP_SUFFIX), $(OBJ_DIR)/%.o, $(SRC))

DIR = $(dir $(OBJ))

# for tests
TESTTILE_OBJ := ./obj/test_tile.o
TESTMMU_OBJ := ./obj/test_mmu.o

# executables
testtile: dir $(OBJ) $(TESTTILE_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(TESTTILE_OBJ) -o $(TESTTILE) $(LIBS)

testmmu: dir $(OBJ) $(TESTMMU_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(TESTMMU_OBJ) -o $(TESTMMU) $(LIBS)

dir:
	mkdir -p $(DIR)

obj/test_tile.o: $(BUILD_DIR)/test_tile.cpp
	$(CC) $(INCLUDE_DIR) $(CFLAGS) -c ./build/test_tile.cpp -o ./obj/test_tile.o

obj/test_mmu.o: $(BUILD_DIR)/test_mmu.cpp
	$(CC) $(INCLUDE_DIR) $(CFLAGS) -c ./build/test_mmu.cpp -o ./obj/test_mmu.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(CPP_SUFFIX)
	$(CC) $(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm $(TESTMMU)
	rm $(TESTTILE)
