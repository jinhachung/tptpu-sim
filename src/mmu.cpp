#include <iostream>
#include <cstdlib>
#include <string>
#include <queue>
#include <assert.h>

#include "common.hpp"
#include "buffer.hpp"
#include "mmu.hpp"

MatrixMultiplyUnit::MatrixMultiplyUnit(int sa_width, int sa_height, int acc_size, UnifiedBuffer *unifiedbuffer, WeightFetcher *weightfetcher) {
    capacity = (float)sa_width * (float)sa_height;
    systolic_array_width = sa_width;
    systolic_array_height = sa_height;
    accumulator_size = acc_size;
    is_main_memory = false;

    idle_cycle = 0;
    busy_cycle = 0;
    wait_cycle = 0;
    total_computation_number = (float)0;
    current_order = 0;

    ub = unifiedbuffer;
    wf = weightfetcher;

    wf_served_queue = new std::vector<request>();
    wf_waiting_queue = new std::vector<request>();
    wf_request_queue = new std::vector<request>();
    ub_served_queue = new std::vector<request>();
    ub_waiting_queue = new std::vector<request>();
    ub_request_queue = new std::vector<request>();
    
    tiling_queue = new std::vector<request>();
}

/* Updates tiling_queue so that all requests in ub_served_queue and wf_served_queue that share
 * the same 'order' value are deleted from their respective queues and pushed into tiling_queue */
void MatrixMultiplyUnit::UpdateTilingQueue() {
    // update tiling_queue
    std::vector<request>::iterator ubit, wfit;
    std::vector<request>::iterator begin = tiling_queue->begin();
    for (ubit = ub_served_queue->begin(); ubit != ub_served_queue->end(); ++ubit) {
        for (wfit = wf_served_queue->begin(); wfit != wf_served_queue->end(); ++wfit) {
            if (ubit->order == wfit->order)
                tiling_queue->push_back(MakeRequest(ubit->order, ubit->size));
        }
    }
    // delete requests moved to tiling_queue from served_queues
    for (; begin != tiling_queue->end(); ++begin) {
        int order = begin->order;
        find_and_delete_by_order(*ub_served_queue, order);
        find_and_delete_by_order(*wf_served_queue, order);
    }
}

bool MatrixMultiplyUnit::IsIdle() {
    UpdateTilingQueue();
    return (tiling_queue->empty() && (wait_cycle == 0));
}

void MatrixMultiplyUnit::Cycle() {
    // update tiling queue and served_queues
    UpdateTilingQueue();
    // check for cycles
    if (wait_cycle == 0) {
        if (!tiling_queue->empty()) {
            // not computing, not ready to compute -> idle
            idle_cycle++;
            return;
        }
        // not computing, but ready to compute-> not idle
        wait_cycle = accumulator_size + systolic_array_width - 1;
        current_order = tiling_queue->front().order;
    }
    // 'compute'
    wait_cycle--;
    busy_cycle++;;
    // check if computation is complete
    if (wait_cycle == 0) {
        // increase total number of computation
        total_computation_number += (float)(2 * systolic_array_width * systolic_array_height * accumulator_size);
        // delete from tiling_queue
        find_and_delete_by_order(*tiling_queue, current_order);
        // delete from Unified Buffer or Weight Fetcher's sender queues
        find_and_delete_by_order(*(ub->GetSenderQueue()), current_order);
        find_and_delete_by_order(*(wf->GetSenderQueue()), current_order);
    }
}

void MatrixMultiplyUnit::PrintStats() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "\tMatrix Multiply Unit Statistics:" << std::endl;
    std::cout << "idle cycles: " << idle_cycle << ", busy cycles: " << busy_cycle << std::endl;
    std::cout << "total number of computations performed by this Matrix Multiply Unit: " << total_computation_number << std::endl;
    std::cout << "======================================================================" << std::endl;
}

