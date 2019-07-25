#include <iostream>
#include <cstdlib>

#include "common.hpp"
#include "controller.hpp"
#include "mmu.hpp"

int main(int argc, char *argv[]) {
    Controller *ctrl = new Controller(10, 10, 20, (MatrixMultiplyUnit *)0);
    ctrl->MatrixMultiply(25, 25, 35, true, 3, (unsigned int)0, (unsigned int)10000);
    ctrl->PrintAllTiles();
    
    return 0;
}
