#include "dram.hpp"

DRAM::DRAM() {
    is_main_memory = true;
    
    sender_queue = new std::vector<request>();
    weight_tile_queue = new std::vector<tile>();
}

DRAM::~DRAM() {
    delete weight_tile_queue;
}

void DRAM::Cycle() {
    ;
}
