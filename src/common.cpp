#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"

request MakeRequest(int order, float size) {
    request req;
    req.order = order;
    req.size = size;

    return req;
}
// functions for std::vector<request>
void pop_front(std::vector<request> &v) {
    assert(v.size() > 0);
    v.erase(v.begin());
}

void find_and_delete_by_order(std::vector<request> &v, int order) {
    std::vector<request>::iterator it;
    for (it = v.begin(); it != v.end(); ++it) {
        if (it->order == order)
            break;
    }
    v.erase(it);
}

// functions for std::vector<float>
void pop_front(std::vector<float> &v) {
    assert(v.size() > 0);
    v.erase(v.begin());
}

float add_all(std::vector<float> &v, int size) {
    assert(v.size() == size);
    std::vector<float>::iterator it;
    float total_value = (float)0;
    for (it = v.begin(); it != v.end(); ++it) {
        total_value += *it;
    }
    return total_value;
}
