#include <iostream>
#include <cstdlib>

#include "buffer.hpp"
#include "common.hpp"
#include "controller.hpp"
#include "interconnect.hpp"
#include "mmu.hpp"
#include "weightfetcher.hpp"

int main(int argc, char *argv[]) {
    std::vector<tile> *wtq = new std::vector<tile>();
    std::vector<tile> *atq = new std::vector<tile>();
    // put random config for Unified Buffer and Weight Fetcher
    UnifiedBuffer *ub = new UnifiedBuffer(30000);
    WeightFetcher *wf = new WeightFetcher(65536, 4);
    MatrixMultiplyUnit *mmu = new MatrixMultiplyUnit(10, 10, 20, ub, wf);

    std::vector<Interconnect *> icnt_list = new std::vector<Interconnect *>();

    Controller *ctrl = new Controller(mmu, icnt_list, wtq, atq);
    ctrl->MatrixMultiply(25, 25, 35, true, 3, (unsigned int)0, (unsigned int)10000);
    ctrl->PrintAllTiles();
    
    return 0;
}
