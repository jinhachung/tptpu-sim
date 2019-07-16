#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"

#pragma once

class Unit {
public:
	virtual void Cycle() = 0;
	virtual bool IsMainMemory() = 0;
	virtual std::vector<request> *GetSenderQueue() = 0;
	virtual std::vector<request> *GetServedQueue() = 0;
	virtual std::vector<request> *GetWaitingQueue() = 0;
	virtual std::vector<request> *GetRequestQueue() = 0;
};
