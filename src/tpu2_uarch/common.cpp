#include "common.hpp"

request MakeRequest(int order, float size) {
    request req;
    req.order = order;
    req.size = size;

    return req;
}

tile MakeTile(int order, unsigned int starting_address, int jump_size, int tile_width, int tile_height, int total_width, int total_height) {
    tile t;
    t.order             = order;
    t.starting_address  = starting_address;
    t.jump_size         = jump_size;
    t.tile_width        = tile_width;
    t.tile_height       = tile_height;
    t.total_width       = total_width;
    t.total_height      = total_height;

    return t;
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
    if (it != v.end())
        v.erase(it);

    // now erase the opposite sign
    for (it = v.begin(); it != v.end(); ++it) {
        if ((it->order + order) == 0)
            break;
    }
    if (it != v.end())
        v.erase(it);
}

// functions for std::vector<tile>
void pop_front(std::vector<tile> &v) {
    assert(v.size() > 0);
    v.erase(v.begin());
}

void find_and_delete_by_order(std::vector<tile> &v, int order) {
    std::vector<tile>::iterator it;
    for (it = v.begin(); it != v.end(); ++it) {
        if (it->order == order)
            break;
    }
    v.erase(it);

    // now erase the opposite sign
    for (it = v.begin(); it != v.end(); ++it) {
        if ((it->order + order) == 0)
            break;
    }
    v.erase(it);
}
