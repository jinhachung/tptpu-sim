#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"
#include "weightfetcher.hpp"

WeightFetcher::WeightFetcher(float _size, int _depth) {
	size = _size;
	depth = _depth;
	is_main_memory = false;
	
	sender_queue = new std::vector<request>();
	served_queue = new std::vector<request>();
	waiting_queue = new std::vector<request>();
	request_queue = new std::vector<request>();
}

void WeightFetcher::Cycle() {
	request req;
	while (!served_queue->empty()) {
		req = served_queue->front();
		pop_front(*served_queue);
		sender_queue->push_back(MakeRequest(req.order, req.size));
	}
}
