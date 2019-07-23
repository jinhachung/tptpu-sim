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

// functions for std::vector<request>
void pop_front(std::vector<request> &v);
void find_and_delete_by_order(std::vector<request> &v, int order);
// functions for std::vector<float>
void pop_front(std::vector<request> &v);
void add_all(std::vector<request> &v, int size);
