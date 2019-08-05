#include "common.hpp"
#include <unistd.h> // for getopt()

int main(int argc, char *argv[]) {
    int opt, channels, ranks, X, Y, Z;
    std::string dram_name, dimension_layout;
    bool is_nchw;
    // set default values:
    channels = 2;
    ranks = 2;
    X = 640;
    Y = 640;
    Z = 1080;
    dram_name = "HBM";
    dimension_layout = "nchw";
    // get values given via CLI
    while((opt = getopt(argc, argv, "c:d:l:r:x:y:z:")) != -1) {
        switch(opt) {
            case 'c':
                channels = atoi(optarg);
                break;
            case 'd':
                dram_name = optarg;
                break;
            case 'l':
                dimension_layout = optarg;
                break;
            case 'r':
                ranks = atoi(optarg);
                break;
            case 'x':
                X = atoi(optarg);
                break;
            case 'y':
                Y = atoi(optarg);
                break;
            case 'z':
                Z = atoi(optarg);
                break;
            default:
                // shouldn't reach here
                std::cerr << "Something wrong with command line arguments..." << std::endl;
                return -1;
        }
    }
    // check for dimension layout
    if (dimension_layout == "nchw")
        is_nchw = true;
    else if (dimension_layout == "nhwc")
        is_nchw = false;
    else {
        std::cerr << "Something wrong with dimension layout: only nchw and nhwc allowed" << std::endl;
        return -1;
    }
    // 24MiB buffer
    float buffer_size = (float)(3 * (1 << 23));
    float clock         = 0.7;                                  // 700MHz
    // the two bw should be changed later to be more flexible and add up to 300GiB/s
    float bw_dram_wf    = 150 * (float)((1 << 30) / 1000000000); // 150GiB/s
    float bw_dram_ub    = 150 * (float)((1 << 30) / 1000000000); // 150GiB/s
    int sa_width = 128;
    int sa_height = 128;
    int accumulator_size = 2048;
    float bw_ub_mmu = 256;  // meaningless
    float bw_wf_mmu = 100;  // meaningless
    UnifiedBuffer *ub = new UnifiedBuffer(buffer_size);
    DRAM *dram = new DRAM(dram_name, clock, channels, ranks);
    WeightFetcher *wf = new WeightFetcher(256 * 256, 4, sa_height);
    Interconnect *dram_ub_icnt = new Interconnect((Unit *)dram, (Unit *)ub, clock, bw_dram_ub, ub->GetCapacity(),
                                                  dram->IsMainMemory(), dram->GetUBSenderQueue(),
                                                  ub->GetServedQueue(), ub->GetWaitingQueue(), ub->GetRequestQueue());
    Interconnect *dram_wf_icnt = new Interconnect((Unit *)dram, (Unit *)wf, clock, bw_dram_wf, wf->GetCapacity(),
                                                  dram->IsMainMemory(), dram->GetWFSenderQueue(),
                                                  wf->GetServedQueue(), wf->GetWaitingQueue(), wf->GetRequestQueue());
    MatrixMultiplyUnit *mmu = new MatrixMultiplyUnit(sa_width, sa_height, accumulator_size, ub, wf);
    Interconnect *ub_mmu_icnt = new Interconnect((Unit *)ub, (Unit *)mmu, clock, bw_ub_mmu, mmu->GetCapacity(),
                                                 ub->IsMainMemory(), ub->GetSenderQueue(),
                                                 mmu->GetUBServedQueue(), mmu->GetUBWaitingQueue(), mmu->GetUBRequestQueue());
    Interconnect *wf_mmu_icnt = new Interconnect((Unit *)wf, (Unit *)mmu, clock, bw_wf_mmu, mmu->GetCapacity(),
                                                 wf->IsMainMemory(), wf->GetSenderQueue(),
                                                 mmu->GetWFServedQueue(), mmu->GetWFWaitingQueue(), mmu->GetWFRequestQueue());
    std::vector<Interconnect *> *icnt_list = new std::vector<Interconnect *>();
    icnt_list->push_back(dram_ub_icnt);
    icnt_list->push_back(dram_wf_icnt);
    icnt_list->push_back(ub_mmu_icnt);
    icnt_list->push_back(wf_mmu_icnt);
    Controller *ctrl = new Controller(mmu, icnt_list, dram->GetWeightTileQueue(), dram->GetActivationTileQueue());
    // setting complete
    // generate request for matrix multiplication
    unsigned int weight_starting_address        = 0xcc000000;
    unsigned int activation_starting_address    = 0xe0000000;
    ctrl->MatrixMultiply(X, Y, Z, is_nchw, 3, weight_starting_address, activation_starting_address);
    
    while (!(dram_ub_icnt->IsIdle() && dram_wf_icnt->IsIdle() && ub_mmu_icnt->IsIdle() && wf_mmu_icnt->IsIdle() && mmu->IsIdle())) {
        mmu->Cycle();
        ub_mmu_icnt->Cycle();
        wf_mmu_icnt->Cycle();
        ub->Cycle();
        wf->Cycle();
        dram_ub_icnt->Cycle();
        dram_wf_icnt->Cycle();
        dram->Cycle();
    }

    // test complete
    dram_ub_icnt->PrintStats("DRAM - Unified Buffer Interconnect");
    dram_wf_icnt->PrintStats("DRAM - Weight Fetcher Interconnect");
    dram->PrintStats();
    mmu->PrintStats();
    return 0;
}
