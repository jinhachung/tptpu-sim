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
    // for UnifiedBuffer and WeightFetcher ()
    virtual std::vector<request> *GetSenderQueue()  {assert(0); return new std::vector<request>();}
    virtual std::vector<request> *GetServedQueue()  {assert(0); return new std::vector<request>();}
    virtual std::vector<request> *GetWaitingQueue() {assert(0); return new std::vector<request>();}
    virtual std::vector<request> *GetRequestQueue() {assert(0); return new std::vector<request>();}
};
