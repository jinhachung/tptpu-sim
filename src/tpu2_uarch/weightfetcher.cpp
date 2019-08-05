#include "weightfetcher.hpp"

WeightFetcher::WeightFetcher(float _size, int _depth, int sa_height) {
    size = _size;
    depth = _depth;
    is_main_memory = false;

    systolic_array_height = sa_height;
    sa_filling_cycle = 0;
    
    sender_queue = new std::vector<request>();
    served_queue = new std::vector<request>();
    waiting_queue = new std::vector<request>();
    request_queue = new std::vector<request>();
}

WeightFetcher::~WeightFetcher() {
    delete sender_queue;
    delete served_queue;
    delete waiting_queue;
    delete request_queue;
}

void WeightFetcher::Cycle() {
    request req;
    if (sa_filling_cycle != 0) {
        // currently flooding (filling the systolic array's double-buffered weight)
        sa_filling_cycle--;
        if (sa_filling_cycle == 0) {
            // finished flooding 
            assert(!served_queue->empty());
            req = served_queue->front();
            pop_front(*served_queue);
            sender_queue->push_back(MakeRequest(req.order, req.size));
        }
        // do nothing if flooding not finished
        return;
    }
    // not flooding
    if ((sender_queue->size() < 2) && (!served_queue->empty())) {
        // flooding available
        sa_filling_cycle = systolic_array_height;
    }
}
