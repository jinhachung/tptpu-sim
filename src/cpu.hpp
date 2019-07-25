#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"

#pragma once

class CPU {
public:
    CPU();
    ~CPU();
    void Cycle();
    
    bool IsMainMemory() {return is_main_memory;}
    std::vector<request> *GetSenderQueue() {return sender_queue;}
    std::vector<tile> *GetActivationTileQueue() {return activation_tile_queue;}
private:
    bool is_main_memory;

    std::vector<request> *sender_queue;
    // shared with Controller
    std::vector<tile> *activation_tile_queue;
};
