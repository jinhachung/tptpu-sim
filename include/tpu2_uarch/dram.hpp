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
    DRAM(std::string name, float frequency, int channels, int ranks);
    ~DRAM();

    double GetFrequencyByName(std::string name);
    void ReceiveRequestSignal(int order, float size);
    int CalculateWaitCycle();
    std::vector<request>::iterator GetNextMemoryRequestIterator();
    void Cycle();

    void PrintStats();

    bool IsMainMemory() {return is_main_memory;}
    bool IsDRAM() {return true;}
    std::string GetDRAMName() {return DRAM_name;}
    double GetDRAMFrequency() {return DRAM_frequency;}
    std::vector<request> *GetUBSenderQueue() {return ub_sender_queue;}
    std::vector<request> *GetWFSenderQueue() {return wf_sender_queue;}
    std::vector<tile> *GetActivationTileQueue() {return activation_tile_queue;}
    std::vector<tile> *GetWeightTileQueue() {return weight_tile_queue;}
    // pure virtual functions
    std::vector<request> *GetSenderQueue() {assert(0); return NULL;}
    std::vector<request> *GetServedQueue() {assert(0); return NULL;}
    std::vector<request> *GetWaitingQueue() {assert(0); return NULL;}
    std::vector<request> *GetRequestQueue() {assert(0); return NULL;}

private:
    bool is_main_memory;
    int idle_cycle;         // cycles DRAM was not bringing in data
    int busy_cycle;         // cycles DRAM was bringing in data
    int wait_cycle;         // cycles DRAM needs to wait for it to fetch data from memory -> calculated via ramulator
    float total_data_size;  // total size of bytes brought in via DRAM
    
    std::string DRAM_name;
    double DRAM_frequency;  // in MHz
    double tptpu_frequency; // in GHz

    std::vector<request> *ub_sender_queue;
    std::vector<request> *wf_sender_queue;
    std::vector<request> *memory_request_queue; // queue of memory requests coming in
    // shared with Controller
    std::vector<tile> *activation_tile_queue;
    std::vector<tile> *weight_tile_queue;
    
    std::vector<request>::iterator servicing_iterator;  // current iterator of memory_request_queue being serviced
};

#endif /* DRAM_H */
