#ifndef WEIGHTFETCHER_H
#define WEIGHTFETCHER_H

#include "common.hpp"
#include "unit.hpp"

struct request;
typedef struct request request;
class Unit;

class WeightFetcher: public Unit {
public:
    WeightFetcher(float _size, int _depth, int sa_height);
    ~WeightFetcher();
    void Cycle();

    bool IsMainMemory() {return is_main_memory;}
    // connected to Matrix Multiply Unit
    std::vector<request> *GetSenderQueue() {return sender_queue;}
    // connected to DRAM
    std::vector<request> *GetServedQueue() {return served_queue;}
    std::vector<request> *GetWaitingQueue() {return waiting_queue;}
    std::vector<request> *GetRequestQueue() {return request_queue;}

    float GetCapacity() {return size * (float)depth;}
private:
    float size;
    int depth;
    bool is_main_memory;

    int systolic_array_height;  // height of the connected systolic array
    int sa_filling_cycle;       // how many cycles the front element of served_queue should wait for it to go to sender_queue

    // connected to Matrix Multiply Unit
    std::vector<request> *sender_queue;
    // connected to DRAM
    std::vector<request> *served_queue;
    std::vector<request> *waiting_queue;
    std::vector<request> *request_queue;
};

#endif /* WEIGHTFETCHER_H */
