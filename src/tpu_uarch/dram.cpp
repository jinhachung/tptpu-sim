#include "dram.hpp"
#include <bits/stdc++.h>
#include <sstream>

DRAM::DRAM(std::string name, float frequency, int channels, int ranks) {
    is_main_memory = true;
    idle_cycle = 0;
    busy_cycle = 0;
    stall_cycle = 0;
    total_data_size = (float)0;

    DRAM_name = name;
    DRAM_frequency = GetFrequencyByName(name);
    tptpu_frequency = (double)frequency;

    sender_queue = new std::vector<request>();
    memory_request_queue = new std::vector<request>();
    weight_tile_queue = new std::vector<tile>();

    // set DRAM standard, channels, ranks, speed and org in dram-config.cfg
    std::string cmdline = "python generate_dram_config.py "
                          + DRAM_name + " " + std::to_string(channels) + " " + std::to_string(ranks);
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

int DRAM::CalculateStallCycle() {
    std::vector<tile>::iterator it;
    //int order, tile_width, tile_height, total_width, total_height, jump_size;
    //unsigned int starting_address;
    //std::string cmd_line;
    int order;
    std::string order_string, tile_width, tile_height, total_width, total_height, jump_size, starting_address, cmd_line;
    std::string file_name;
    order = memory_request_queue->front().order;
    // search tile queue for tile information
    for (it = weight_tile_queue->begin(); it != weight_tile_queue->end(); ++it) {
        if (order == it->order)
            break;
    }
    // we must have found the correct tile
    assert(it != weight_tile_queue->end());

    // set values
    tile_width          = std::to_string(it->tile_width);
    tile_height         = std::to_string(it->tile_height);
    total_width         = std::to_string(it->total_width);
    total_height        = std::to_string(it->total_height);
    jump_size           = std::to_string(it->jump_size);
    starting_address    = std::to_string(it->starting_address);
    order_string        = std::to_string(order);
    // set command to generate instruction trace
    cmd_line = "python generate_instruction.py " + starting_address
               + " " + jump_size + " " + tile_width + " " + tile_height
               + " " + total_width + " " + total_height + " " + order_string + " dram";
    system(cmd_line.c_str());
    
    // set command to run ramulator
    cmd_line = "./../ramulator/ramulator dram-config.cfg --mode=dram --stats ./ramulator_output/"
               + order_string + ".output.txt ./build/dram/" + order_string + ".trace";
    system(cmd_line.c_str());
    
    // now the result is written to ramulator_output/order.output.txt
    file_name = "ramulator_output/" + order_string + ".output.txt";
    // this part is ad hoc, taken from StackOverflow: "C++ searching text file for
    // a particular string and returning the line number where that string is on"
    std::ifstream fileInput;
    std::string line = "not found yet";
    std::string searchstring = "ramulator.dram_cycles";
    const char *search = searchstring.c_str();
    
    // open file
    fileInput.open(file_name);
    if (!fileInput.is_open()) {
        std::cerr << "File " << file_name << " cannot be opened." << std::endl;
        assert(0);
    }

    // look for "ramulator.dram_cycles" in file
    while(getline(fileInput, line)) {
        if (line.find(search, 0) != std::string::npos) {
            // found
            break;
        }
    }

    // close file
    fileInput.close();

    // now, line should hold the string with the number of total dram cycles simulated
    // this part is ad hoc, taken from https://www.geeksforgeeks.org/extract-integers-string-c/
    std::stringstream ss;
    ss << line;
    std::string temp;
    int ramulator_dram_cycles = -1;
    int tptpu_dram_cycles;
    while (!ss.eof()) {
        ss >> temp;
        if (std::stringstream(temp) >> ramulator_dram_cycles) {
            break;
        }
    }
    // check for correctness of ramulator.dram_cycles
    if (ramulator_dram_cycles == -1) {
        std::cerr << "Could not find 'ramulator.dram_cycles' in file " << file_name << std::endl;
        assert(0);
    }

    // now calculate tptpu's dram_cycles
    tptpu_dram_cycles = (int)((ramulator_dram_cycles * tptpu_frequency * (double)1000 - 1) / DRAM_frequency) + 1;

    return tptpu_dram_cycles;
}

void DRAM::Cycle() {
    if (stall_cycle == 0) {
        if (memory_request_queue->empty()) {
            // not bringing in data, not requested to bring in either -> idle
            idle_cycle++;
            return;
        }
        // not bring in data, but has been requested to -> not idle
        stall_cycle = CalculateStallCycle();
    }
    // 'bring in data'
    stall_cycle--;
    busy_cycle++;
    // check if data transfer is complete
    if (stall_cycle == 0) {
        // delete from memory_request_queue
        request req = memory_request_queue->front();
        pop_front(*memory_request_queue);
        // ready to send -> add to sender_queue
        sender_queue->push_back(MakeRequest(req.order, req.size));
        total_data_size += (float)req.size;
    }
    // do nothing if data transfer is not complete yet
}

void DRAM::PrintStats() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "\t\t" << DRAM_name << " DRAM Statistics:" << std::endl;
    std::cout << "\tidle cycles: " << idle_cycle << ",\t\tbusy cycles: " << busy_cycle << std::endl;
    std::cout << "total bytes brought in from this DRAM:\t\t\t" << total_data_size << std::endl;
    std::cout << "======================================================================" << std::endl;
}

