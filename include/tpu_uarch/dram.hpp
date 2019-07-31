#ifndef DRAM_H
#define DRAM_H

#include "common.hpp"
#include "unit.hpp"

struct request;
typedef struct request request;
struct tile;
typedef struct tile tile;

class DRAM: public Unit {
public:
    DRAM(std::string name);
    ~DRAM();

    double GetFrequencyByName(std::string name);

    void Cycle();

    bool IsMainMemory() {return is_main_memory;}
    bool IsDRAM() {return true;}
    std::string GetDRAMName() {return DRAM_name;}
    double GetDRAMFrequency() {return DRAM_frequency;}
    std::vector<request> *GetSenderQueue() {return sender_queue;}
    std::vector<tile> *GetWeightTileQueue() {return weight_tile_queue;}
    // pure virtual functions
    std::vector<request> *GetServedQueue() {assert(0); return NULL;}
    std::vector<request> *GetWaitingQueue() {assert(0); return NULL;}
    std::vector<request> *GetRequestQueue() {assert(0); return NULL;}

private:
    bool is_main_memory;

    std::string DRAM_name;
    double DRAM_frequency;

    std::vector<request> *sender_queue;
    // shared with Controller
    std::vector<tile> *weight_tile_queue;
};

#endif /* DRAM_H */
