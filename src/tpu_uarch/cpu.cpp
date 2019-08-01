#include "cpu.hpp"

CPU::CPU() {
    is_main_memory = true;

    sender_queue = new std::vector<request>();
    memory_request_queue = new std::vector<request>();
    activation_tile_queue = new std::vector<tile>();
}

CPU::~CPU() {
    delete activation_tile_queue;
}

void CPU::ReceiveRequestSignal(int order, float size) {
    memory_request_queue->push_back(MakeRequest(order, size));
}

void CPU::Cycle() {
    // CPU has all data ready
    if (!memory_request_queue->empty()) {
        request req = memory_request_queue->front();
        sender_queue->push_back(MakeRequest(req.order, req.size));
        pop_front(*memory_request_queue);
    }
}
