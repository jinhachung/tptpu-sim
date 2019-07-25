#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"
#include "cpu.hpp"

CPU::CPU() {
    is_main_memory = true;

    sender_queue = new std::vector<request>();
    activation_tile_queue = new std::vector<tile>();
}

CPU::~CPU() {
    delete activation_tile_queue;
}

void CPU::Cycle() {
    ;
}
