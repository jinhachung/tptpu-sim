#ifndef CPU_H
#define CPU_H

#include "common.hpp"
#include "unit.hpp"

struct request;
typedef struct request request;
struct tile;
typedef struct tile tile;

class CPU: public Unit {
public:
    CPU();
    ~CPU();
    void Cycle();
    
    bool IsMainMemory() {return is_main_memory;}
    std::vector<request> *GetSenderQueue() {return sender_queue;}
    std::vector<tile> *GetActivationTileQueue() {return activation_tile_queue;}
    // pure virtual functions
    std::vector<request> *GetServedQueue() {assert(0); return NULL;}
    std::vector<request> *GetWaitingQueue() {assert(0); return NULL;}
    std::vector<request> *GetRequestQueue() {assert(0); return NULL;}
private:
    bool is_main_memory;

    std::vector<request> *sender_queue;
    // shared with Controller
    std::vector<tile> *activation_tile_queue;
};

#endif /* CPU_H */
