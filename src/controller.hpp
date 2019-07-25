#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"
#include "dram.hpp"
#include "mmu.hpp"

#pragma once

class Controller {
public:
    Controller(MatrixMultiplyUnit *matrixmultiplyunit,
               std::vector<tile> *weighttilequeue, std::vector<tile> *activationtilequeue);
    void Tile(int A, int B, int C, bool is_dimension_nchw, int channel,
              unsigned int address_X, unsigned int address_Y);
    void PushRequestsFromTiles();
    void MatrixMultiply(int A, int B, int C, bool is_dimension_nchw, int channel,
                        unsigned int address_X, unsigned int address_Y);
    void PrintAllTiles();
private:
    int systolic_array_width;
    int systolic_array_height;
    int accumulator_size;
    MatrixMultiplyUnit *mmu;

    int id;             // number to set for next request's order in tiling process

    std::vector<tile> *weight_tile_queue;
    std::vector<tile> *activation_tile_queue;
};
