#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"
#include "dram.hpp"

DRAM::DRAM() {
	is_main_memory = true;
	sender_queue = new std::vector<request>();
}

void DRAM::Cycle() {
	;
}
