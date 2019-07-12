#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

struct request {
    int order;
    float size;
};
typedef struct request request;

request MakeRequest(int order, float size) {
    request req;
    req.order = order;
    req.size = size;

    return req;
}

template<typename T>
void pop_front(std::vector<T> &v)
{
    assert(v.size() > 0);
    v.erase(v.begin());
}
