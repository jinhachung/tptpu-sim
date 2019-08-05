#ifndef ICNT_H
#define ICNT_H

#include "common.hpp"

struct request;
typedef struct request request;
class Unit;
class Controller;

class Interconnect {
public:
    Interconnect(Unit *_sender, Unit *_receiver, float _clock, float _bw, float _receiver_capacity,
                 bool _is_sender_main_memory, std::vector<request> *senderqueue, std::vector<request> *servedqueue,
                 std::vector<request> *waitingqueue, std::vector<request> *requestqueue);
    // pushes the given request into receiver's waiting queue
    void ReceiveRequest(request req);
    bool ReceiverFull();
    float ReceiversSenderQueueSize();

    bool IsIdle();
    void Cycle();
    void PrintStats(std::string name);
    
    void SetController(Controller *ctrl) {controller = ctrl;}

    Unit *GetSender() {return sender;}
    Unit *GetReceiver() {return receiver;}

    std::vector<request> *GetSenderQueue() {return sender_queue;}
    std::vector<request> *GetServedQueue() {return served_queue;}
    std::vector<request> *GetWaitingQueue() {return waiting_queue;}
    std::vector<request> *GetRequestQueue() {return request_queue;}
private:  
    Unit *sender;                           // pointer to the sender of this interconnect
    Unit *receiver;                         // pointer to the receiver of this interconnect
    float clock;                            // clock in GHz
    float bw;                               // bandwidth in GB/s
    float bpc;                              // bytes sent per cycle
    float receiver_capacity;                // total amount of bytes the receiver can endure (store) up to
    bool is_sender_main_memory;             // whether the sender is main memory or not

    int idle_cycle;                         // number of cycles that the interconnect was idle
    int busy_cycle;                         // number of cycles that the interconnect was busy
    float sent_size;                        // total number of bytes sent over this interconnect

    Controller *controller;                 // pointer to Controller

    // sender queue (one who serves)
    std::vector<request> *sender_queue;     // vector of requests that sender has left to send to receiver (size changes)
    // receiver queues (one who requests)
    std::vector<request> *served_queue;     // vector of requests that the receiver has been serviced
    std::vector<request> *waiting_queue;    // vector of requests that the receiver has to receive from sender
    std::vector<request> *request_queue;    // vector of requests that the receiver needs to request to sender
};

#endif /* ICNT_H */
