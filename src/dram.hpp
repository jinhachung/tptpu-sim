#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"

#pragma once

class DRAM {
public:
	DRAM();
	void Cycle();

	bool IsMainMemory() {return is_main_memory;}
	std::vector<request> *GetSenderQueue() {return sender_queue;}
private:
	bool is_main_memory;

	std::vector<request> *sender_queue;
};
