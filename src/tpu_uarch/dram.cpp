#include "dram.hpp"
#include <vector>
#include <bits/stdc++.h>

DRAM::DRAM(std::string name) {
    is_main_memory = true;
    stall_cycle = 0;
    
    DRAM_name = name;
    DRAM_frequency = GetFrequencyByName(name);

    sender_queue = new std::vector<request>();
    memory_request_queue = new std::vector<request>();
    weight_tile_queue = new std::vector<tile>();

    // set DRAM name in dram-config.cfg
    std::string cmdline = "python generate_dram_config.py " + DRAM_name;
    system(cmdline.c_str());
}

DRAM::~DRAM() {
    delete weight_tile_queue;
}

double DRAM::GetFrequencyByName(std::string name) {
    // DDR3
    if ((name == "DDR3_800D") || (name == "DDR3_800E"))
        return (400.0 / 3) * 3;
    if ((name == "DDR3_1066E") || (name == "DDR3_1066F") || (name == "DDR3_1066G"))
        return (400.0 / 3) * 4;
    if ((name == "DDR3_1333G") || (name == "DDR3_1333H"))
        return (400.0 / 3) * 5;
    if ((name == "DDR3_1600H") || (name == "DDR3_1600J") || (name == "DDR3_1600K"))
        return (400.0 / 3) * 6;
    if ((name == "DDR3_1866K") || (name == "DDR3_1866L"))
        return (400.0 / 3) * 7;
    if ((name == "DDR3_2133L") || (name == "DDR3_2133M"))
        return (400.0 / 3) * 8;
    // DDR4
    if ((name == "DDR4_1600K") || (name == "DDR4_1600L"))
        return (400.0 / 3) * 6;
    if ((name == "DDR4_1866M") || (name == "DDR4_1866N"))
        return (400.0 / 3) * 7;
    if ((name == "DDR4_2133P") || (name == "DDR4_2133R"))
        return (400.0 / 3) * 8;
    if ((name == "DDR4_2400R") || (name == "DDR4_2400U"))
        return (400.0 / 3) * 9;
    if (name == "DDR4_3200")
        return 1600.0;
    // should not reach here
    std::cerr << "*** Invalid DRAM name: " << name << " ***" << std::endl;
    assert(0);
}

void DRAM::ReceiveRequestSignal(int order, float size) {
    memory_request_queue->push_back(MakeRequest(order, size));
}

void DRAM::Cycle() {
    // only for easy debugging... will change to use ramulator later on
    if (!memory_request_queue->empty()) {
        request req = memory_request_queue->front();
        sender_queue->push_back(MakeRequest(req.order, req.size));
        pop_front(*memory_request_queue);
    }
}
