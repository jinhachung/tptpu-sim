#include "interconnect.hpp"

Interconnect::Interconnect(Unit *_sender, Unit *_receiver, float _clock, float _bw, float _receiver_capacity,
                           bool _is_sender_main_memory, std::vector<request> *senderqueue, std::vector<request> *servedqueue,
                           std::vector<request> *waitingqueue, std::vector<request> *requestqueue) {
    sender = _sender;
    receiver = _receiver;
    clock = _clock;
    bw = _bw;
    bpc = bw / clock;
    receiver_capacity = _receiver_capacity;
    is_sender_main_memory = _is_sender_main_memory;

    idle_cycle = 0;
    busy_cycle = 0;
    sent_size = 0;

    controller = NULL;

    sender_queue = senderqueue;
    served_queue = servedqueue;
    waiting_queue = waitingqueue;
    request_queue = requestqueue;
}

/* Called when another request is made from the receiver.
 * The request is pushed into the receiver's request_queue, and taken care of in Cycle() */
void Interconnect::ReceiveRequest(request req) {
    //request r = req;
    request_queue->push_back(MakeRequest(req.order, req.size));
}

/* Checks to see if the receiver buffer is not overwhelmed with data
 * Returns true if receiver cannot receive any more data, false otherwise
 * Should be called when at least one of waiting_queue and sender_queue is not empty
 * But waiting_queue is empty only when sender_queue is empty,
 * so we should make sure sender_queue is nonempty */
bool Interconnect::ReceiverFull() {
    assert(!sender_queue->empty());

    float totalsize = ReceiversSenderQueueSize();
    std::vector<request>::iterator it;
    for (it = served_queue->begin(); it != served_queue->end(); ++it) {
        totalsize += it->size;
    }
    int current_index = sender_queue->front().order;
    for (it = waiting_queue->begin(); it != waiting_queue->end(); ++it) {
        if (it->order == current_index) {
            totalsize += it->size;
            break;
        }
    }
    // if not found, then this is the start the transmission of new data
    if (it == waiting_queue->end())
        totalsize += sender_queue->front().size;
    // totalsize is amount of data in receiver if we finish the transmission about to start right now
    return (receiver_capacity < totalsize);
}

/* Returns the total size of data stored in the sender_queue of receiver
 * This is needed so that ReceiverFull() can correctly determine the total size of data that the receiver holds
 * With this function, non-main-mem senders like UnifiedBuffer and WeightFetcher can
 * move the requests from their served_queue to sender_queue in their respective Cycle()s 
 * This function is called in ReceiverFull() */
float Interconnect::ReceiversSenderQueueSize() {
    if (receiver->IsMatrixMultiplyUnit())
        return (float)0;
    // sender_queue of the interconnect's receiver
    std::vector<request> *receivers_sender_queue = receiver->GetSenderQueue();
    std::vector<request>::iterator it;
    float totalsize = 0;
    for (it = receivers_sender_queue->begin(); it != receivers_sender_queue->end(); ++it) {
        totalsize += it->size;
    }
    return totalsize;
}

bool Interconnect::IsIdle() {
    return (sender_queue->empty() && served_queue->empty() && waiting_queue->empty() && request_queue->empty());
}

void Interconnect::Cycle() {
    std::vector<request>::iterator it;
    // send all pending requests in request_queue to sender
    while (!request_queue->empty()) {
        request req = MakeRequest(request_queue->front().order, request_queue->front().size);
        waiting_queue->push_back(req);
        pop_front(*request_queue);
        // signal sender for main-memory units
        if (is_sender_main_memory)
            sender->ReceiveRequestSignal(req.order, req.size);
    }

    // special case when the receiver is Matrix Multiply Unit
    if (receiver->IsMatrixMultiplyUnit()) {
        // copy all wait_queue items to sender's request_queue (no duplicates)
        std::vector<request>::iterator rwqit, swqit, srqit;
        int order;
        float size;
        // duplicate check
        for (rwqit = waiting_queue->begin(); rwqit != waiting_queue->end(); ++rwqit) {
            order = rwqit->order;
            size = rwqit->size;
            // check for sender's waiting_queue
            for (swqit = sender->GetWaitingQueue()->begin(); swqit != sender->GetWaitingQueue()->end(); ++swqit) {
                if (order == swqit->order)
                    break;
            }
            for (srqit = sender->GetRequestQueue()->begin(); srqit != sender->GetRequestQueue()->end(); ++srqit) {
                if (order == srqit->order)
                    break;
            }
            if ((swqit == sender->GetWaitingQueue()->end()) && (srqit == sender->GetRequestQueue()->end())) {
                // not found -> copy to sender's request_queue, after checking Matrix Multiply Unit's tiling_queue
                std::vector<request> *tiling_queue = receiver->GetTilingQueue();
                std::vector<request>::iterator it;
                for (it = tiling_queue->begin(); it != tiling_queue->end(); ++it) {
                    if (order == it->order)
                        break;
                }
                // not in tiling_queue -> new request!
                if (it == tiling_queue->end())
                    sender->GetRequestQueue()->push_back(MakeRequest(order, size));
                else {
                    // cannot reach here (probably)
                    assert(0);
                }
            }
        }
        // take care of cycle
        if (sender_queue->empty())
            idle_cycle++;
        else
            busy_cycle++;

        return;
    }

    // cycle count for interconnects other than ones that send to Matrix Multiply Unit
    if (sender_queue->empty() || ReceiverFull())
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
            pop_front(*sender_queue);
            for (it = waiting_queue->begin(); it != waiting_queue->end(); ++it) {
                if (it->order == order) {
                    bts = it->size;
                    break;
                }
            }
            // update sent_size
            sent_size += bts;
            if (it != waiting_queue->end()) {
                // found
                waiting_queue->erase(it);
                served_queue->push_back(MakeRequest(order, bts));
                // Erase value if the receiver is sender to Matrix Multiply Unit!
                controller->RaiseDoneSignal(receiver, order);
            }
            else {
                // not found... something wrong
                assert(0);
            }
        }
    }
}

void Interconnect::PrintStats(std::string name) {
   std::cout << "======================================================================" << std::endl;
   std::cout << "\t\t" << name << " Statistics:" << std::endl;
   std::cout << "\tidle cycles: " << idle_cycle << ",\t\tbusy cycles: " << busy_cycle << std::endl;
   std::cout << "total bytes sent over this interconnect:\t\t" << sent_size << std::endl;
   std::cout << "======================================================================" << std::endl;
}
