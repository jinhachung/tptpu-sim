#ifndef COMMON_H
#define COMMON_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "buffer.hpp"
#include "controller.hpp"
#include "cpu.hpp"
#include "dram.hpp"
#include "interconnect.hpp"
#include "mmu.hpp"
#include "unit.hpp"
#include "weightfetcher.hpp"

struct request {
    int order;
    float size;
};
typedef struct request request;

struct tile {
    int order;
    unsigned int starting_address;
    int jump_size;
    int tile_width;
    int tile_height;
    int total_width;
    int total_height;
};
typedef struct tile tile;

request MakeRequest(int order, float size);
tile MakeTile(int order, unsigned int starting_address, int jump_size, int tile_width, int tile_height, int total_width, int total_height);

// functions for std::vector<request>
void pop_front(std::vector<request> &v);
void find_and_delete_by_order(std::vector<request> &v, int order);
// functions for std::vector<tile>
void pop_front(std::vector<tile> &v);
void find_and_delete_by_order(std::vector<tile> &v, int order);

#endif /* COMMON_H */
