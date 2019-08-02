#include "common.hpp"
#include <unistd.h> // for getopt()

int main(int argc, char *argv[]) {
    /*
    if (argc != 15) {
        std::cerr << "14 arguments should be given:" << std::endl
                  << "-d -c -r for DRAM type, channel number, rank number," << std::endl
                  << "-x -y -z for matrix dimensions: X-by-Y * Y-by-Z matrix multiplication, and" << std::endl
                  << "-l for dimension layout (nchw or nhwc)" << std::endl;
        return -1;
    }
    */
    int opt, channels, ranks, X, Y, Z;
    std::string dram_name, dimension_layout;
    bool is_nchw;
    // set default values:
    channels = 2;
    ranks = 2;
    X = 640;
    Y = 640;
    Z = 1080;
    dram_name = "DDR3_1600K";
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
    CPU *cpu = new CPU();
    // 256 * 48KiB (12 * 2^20B) per buffer, 256 * 96KiB total (24MiB)
    float buffer_size = (float)(3 * (1 << 22));
    UnifiedBuffer *ub = new UnifiedBuffer(buffer_size);
    float clock         = 0.7;                                  // 700MHz
    float bw_dram_wf    = 30 * (float)((1 << 30) / 1000000000); // 30GiB/s
    float bw_cpu_ub     = 10 * (float)((1 << 30) / 1000000000); // 10GiB/s
    DRAM *dram = new DRAM(dram_name, clock, channels, ranks);
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
    unsigned int weight_starting_address        = 0xcc000000;
    unsigned int activation_starting_address    = 0xe0000000;
    ctrl->MatrixMultiply(X, Y, Z, is_nchw, 3, weight_starting_address, activation_starting_address);
    
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
