#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"
#include "interconnect.hpp"

Interconnect::Interconnect(void *_sender, void *_receiver, float _clock, float _bw,
                           std::vector<request> *senderqueue, std::vector<request> *servedqueue,
                           std::vector<request> *waitingqueue, std::vector<request> *requestqueue) {
    sender = _sender;
    receiver = _receiver;
    clock = _clock;
    bw = _bw;

    bpc = bw / clock;

    idle_cycle = 0;
    busy_cycle = 0;

    sender_queue = senderqueue;
    served_queue = servedqueue;
    waiting_queue = waitingqueue;
    request_queue = requestqueue;
}


/* Called when another request is made from the receiver.
 * The request is pushed into the receiver's request_queue, and taken care of in Cycle()  */
void Interconnect::ReceiveRequest(request req) {
    //request r = req;
    request_queue->push_back(MakeRequest(req.order, req.size));
}


void Interconnect::Cycle() {
    std::vector<request>::iterator it;
    // send all pending requests in request_queue to sender
    while (!request_queue->empty()) {
        request req = MakeRequest(request_queue->front().order, request_queue->front().size);
        waiting_queue->push_back(req);
        sender_queue->push_back(req);
        pop_front(request_queue);
    }
    
    // cycle count
    if (sender_queue->empty())
        idle_cycle++;
    else {
        busy_cycle++;
        // sender sends data
        int order = sender_queue->front().order;
        float bts = sender_queue->front().size;
        bts = ((bts - bpc) < 0) ? 0 : (bts - bpc);
        // take care of request in queue
        if (bts != 0)
            sender_queue->front().size = bts;
        else {
            // the sender is done sending
            // from here on, bts holds value of the original requested size
            pop_front(sender_queue);
            for (it = waiting_queue->begin(); it != waiting_queue->end(); ++it) {
                if (it->order == order) {
                    bts = it->size;
                    break;
                }
            }
            if (it != waiting_queue->end()) {
                // found
                waiting_queue->erase(it);
                served_queue->push_back(MakeRequest(order, bts));
            }
            else {
                // not found... something wrong
                assert(0);
            }
        }
    }
}


