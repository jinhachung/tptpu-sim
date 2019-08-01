#include "common.hpp"

int main(int argc, char *argv[]) {
    CPU *cpu = new CPU();
    // 256 * 48KiB (12 * 2^20B) per buffer, 256 * 96KiB total (24MiB)
    float buffer_size = (float)(3 * (1 << 22));
    UnifiedBuffer *ub = new UnifiedBuffer(buffer_size);
    float clock         = 0.7;                              // 700MHz
    float bw_dram_wf    = 30 * (float)((1 << 30) / (10^9)); // 30GiB/s
    float bw_cpu_ub     = 10 * (float)((1 << 30) / (10^9)); // 10GiB/s
    int channels = 1;
    int ranks = 1;
    // DDR3-1600K
    DRAM *dram = new DRAM("DDR3_1600K", clock, channels, ranks);
    // 64KiB per tile, 4 tiles deep
    WeightFetcher *wf = new WeightFetcher(256 * 256, 4);
    Interconnect *cpu_ub_icnt = new Interconnect((Unit *)cpu, (Unit *)ub, clock, bw_cpu_ub, ub->GetCapacity(),
                                                 cpu->IsMainMemory(), cpu->GetSenderQueue(),
                                                 ub->GetServedQueue(), ub->GetWaitingQueue(), ub->GetRequestQueue());
    Interconnect *dram_wf_icnt = new Interconnect((Unit *)dram, (Unit *)wf, clock, bw_dram_wf, wf->GetCapacity(),
                                                  dram->IsMainMemory(), dram->GetSenderQueue(),
                                                  wf->GetServedQueue(), wf->GetWaitingQueue(), wf->GetRequestQueue());
    // 256 by 256 systolic array, accumulator size 2048
    int sa_width = 256;
    int sa_height = 256;
    int accumulator_size = 2048;
    float bw_ub_mmu = 256;  // meaningless
    float bw_wf_mmu = 100;  // meaningless
    MatrixMultiplyUnit *mmu = new MatrixMultiplyUnit(sa_width, sa_height, accumulator_size, ub, wf);
    Interconnect *ub_mmu_icnt = new Interconnect((Unit *)ub, (Unit *)mmu, clock, bw_ub_mmu, mmu->GetCapacity(),
                                                 ub->IsMainMemory(), ub->GetSenderQueue(),
                                                 mmu->GetUBServedQueue(), mmu->GetUBWaitingQueue(), mmu->GetUBRequestQueue());
    Interconnect *wf_mmu_icnt = new Interconnect((Unit *)wf, (Unit *)mmu, clock, bw_wf_mmu, mmu->GetCapacity(),
                                                 wf->IsMainMemory(), wf->GetSenderQueue(),
                                                 mmu->GetWFServedQueue(), mmu->GetWFWaitingQueue(), mmu->GetWFRequestQueue());
    std::vector<Interconnect *> *icnt_list = new std::vector<Interconnect *>();
    icnt_list->push_back(cpu_ub_icnt);
    icnt_list->push_back(dram_wf_icnt);
    icnt_list->push_back(ub_mmu_icnt);
    icnt_list->push_back(wf_mmu_icnt);
    Controller *ctrl = new Controller(mmu, icnt_list, dram->GetWeightTileQueue(), cpu->GetActivationTileQueue());
    // setting complete
    // generate request for matrix multiplication
    unsigned int weight_starting_address = 3422552064;      // 0xcc000000 = 3422552064
    unsigned int activation_starting_address = 3758096384;  // 0xe0000000 = 3758096384
    ctrl->MatrixMultiply(640, 640, 1080, true, 3, weight_starting_address, activation_starting_address);
    
    while (!(cpu_ub_icnt->IsIdle() && dram_wf_icnt->IsIdle() && ub_mmu_icnt->IsIdle() && wf_mmu_icnt->IsIdle() && mmu->IsIdle())) {
        mmu->Cycle();
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
    dram->PrintStats();
    mmu->PrintStats();
    return 0;
}
