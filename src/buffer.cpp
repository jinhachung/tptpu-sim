#include "buffer.hpp"

UnifiedBuffer::UnifiedBuffer(float buffer_size) {
    capacity = 2 * buffer_size;
    is_main_memory = false;

    sender_queue = new std::vector<request>();
    served_queue = new std::vector<request>();
    waiting_queue = new std::vector<request>();
    request_queue = new std::vector<request>();
}

void UnifiedBuffer::Cycle() {
    request req;
    while (!served_queue->empty()) {
        req = served_queue->front();
        pop_front(*served_queue);
        sender_queue->push_back(MakeRequest(req.order, req.size));
    }
}

