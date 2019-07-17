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

void pop_front(std::vector<request> &v)
{
    assert(v.size() > 0);
    v.erase(v.begin());
}
