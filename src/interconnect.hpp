#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"

#pragma once

class Interconnect {
public:
    Interconnect(void *_sender, void *_receiver, float _clock, float _bw,
                 std::vector<request> *senderqueue, std::vector<request> *servedqueue,
                 std::vector<request> *waitingqueue, std::vector<request> *requestqueue);
    // pushes the given request into receiver's waiting queue
    void ReceiveRequest(request req);
    void Cycle();

    std::vector<request>* GetSenderQueue() {return sender_queue;}
    std::vector<request>* GetServedQueue() {return served_queue;}
    std::vector<request>* GetWaitingQueue() {return waiting_queue;}
    std::vector<request> *GetRequestQueue() {return request_queue;}
private:  
    void *sender;                           // pointer to the sender of this interconnect
    void *receiver;                         // pointer to the receiver of this interconnect
    float clock;                            // clock in GHz
    float bw;                               // bandwidth in GB/s
    float bpc;                              // bytes sent per cycle

    int idle_cycle;                         // number of cycles that the interconnect was idle
    int busy_cycle;                         // number of cycles that the interconnect was busy

    // sender queue (one who serves)
    std::vector<request> *sender_queue;     // vector of requests that sender has left to send to receiver (size changes)
    // receiver queues (one who requests)
    std::vector<request> *served_queue;     // vector of requests that the receiver has been serviced
    std::vector<request> *waiting_queue;    // vector of requests that the receiver has to receive from sender
    std::vector<request> *request_queue;    // vector of requests that the receiver needs to request to sender
};
