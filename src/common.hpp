#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#pragma once

struct request {
    int order;
    float size;
};
typedef struct request request;

request MakeRequest(int order, float size);

void pop_front(std::vector<request> &v);
