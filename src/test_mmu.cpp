#include <iostream>
#include <cstdlib>

#include "buffer.hpp"
#include "common.hpp"
#include "controller.hpp"
#include "cpu.hpp"
#include "dram.hpp"
#include "interconnect.hpp"
#include "mmu.hpp"
#include "unit.hpp"
#include "weightfetcher.hpp"

int main(int argc, char *argv[]) {
    CPU *cpu = new CPU();
    // 30KB per buffer, 60KB total
    UnifiedBuffer *ub = new UnifiedBuffer(30000);
    DRAM *dram = new DRAM();
    // 64KiB per tile, 4 tiles deep
    WeightFetcher *wf = new WeightFetcher(65536, 4);
    float clock = 1;        // 1GHz
    float bw = 1000;        // 1000GB/s
    Interconnect *cpu_ub_icnt = new Interconnect((Unit *)cpu, (Unit *)ub, clock, bw, ub->GetCapacity(),
                                                 cpu->IsMainMemory(), cpu->GetSenderQueue(),
                                                 ub->GetServedQueue(), ub->GetWaitingQueue(), ub->GetRequestQueue());
    Interconnect *dram_wf_icnt = new Interconnect((Unit *)dram, (Unit *)wf, clock, bw, wf->GetCapacity(),
                                                  dram->IsMainMemory(), dram->GetSenderQueue(),
                                                  wf->GetServedQueue(), wf->GetWaitingQueue(), wf->GetRequestQueue());
    // 256 by 256 systolic array, accumulator size 2048
    int sa_width = 256;
    int sa_height = 256;
    int accumulator_size = 2048;
    float bw_ub_mmu = 256;  // probably meaningless
    float bw_wf_mmu = 100;  // probably meaningless
    MatrixMultiplyUnit *mmu = new MatrixMultiplyUnit(sa_width, sa_height, accumulator_size, ub, wf);
    Interconnect *ub_mmu_icnt = new Interconnect((Unit *)ub, (Unit *)mmu, clock, bw_ub_mmu, mmu->GetCapacity(),
                                                 ub->IsMainMemory(), ub->GetSenderQueue(),
                                                 mmu->GetUBServedQueue(), mmu->GetUBWaitingQueue(), mmu->GetUBRequestQueue());
    Interconnect *wf_mmu_icnt = new Interconnect((Unit *)wf, (Unit *)mmu, clock, bw_wf_mmu, mmu->GetCapacity(),
                                                 wf->IsMainMemory(), wf->GetSenderQueue(),
                                                 mmu->GetWFServedQueue(), mmu->GetWFWaitingQueue(), mmu->GetWFRequestQueue());
    Controller *ctrl = new Controller(mmu, dram->GetWeightTileQueue(), cpu->GetActivationTileQueue());
    // setting complete
    // generate request for matrix multiplication
    ctrl->MatrixMultiply(640, 640, 1080, true, 3, (unsigned int)0, (unsigned int)100000000);
    
    while (!( cpu_ub_icnt->IsIdle() && dram_wf_icnt->IsIdle() && ub_mmu_icnt->IsIdle()
              && wf_mmu_icnt->IsIdle() && mmu->IsIdle() )) {
        mmu->Cycle();
        // right order?
        ub_mmu_icnt->Cycle();
        wf_mmu_icnt->Cycle();
        ub->Cycle();
        wf->Cycle();
        cpu_ub_icnt->Cycle();
        dram_wf_icnt->Cycle();
        cpu->Cycle();
        dram->Cycle();
    }

    // test complete
    cpu_ub_icnt->PrintStats("CPU - Unified Buffer Interconnect");
    dram_wf_icnt->PrintStats("DRAM - Weight Fetcher Interconnect");
    ub_mmu_icnt->PrintStats("Unified Buffer - Matrix Multiply Unit Interconnect");
    wf_mmu_icnt->PrintStats("Weight Fetcher - Matrix Multiply Unit Interconnect");
    mmu->PrintStats();
    return 0;
}
