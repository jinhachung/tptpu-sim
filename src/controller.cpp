#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>

#include "common.hpp"
#include "controller.hpp"
#include "unit.hpp"

Controller::Controller(int sa_width, int sa_height, int acc_size, MatrixMultiplyUnit *matrixmultiplyunit) {
    systolic_array_width = sa_width;
    systolic_array_height = sa_height;
    accumulator_size = acc_size;
    mmu = matrixmultiplyunit;

    id = 1;

    weight_tile_queue = new std::vector<tile>();
    activation_tile_queue = new std::vector<tile>();
}

/* This functions takes two matrices X and Y (A by B and B by C) and tiles them into
 * systolic_array_width by systolic_array_height and systolic_array_width by accumulator_size matrices respectively
 * Then the tiles of corresponding sizes are pushed into request_queues of Matrix Multiply Unit
 * In this case, the weight matrix from Weight Fetcher is the matrix X, starting at address address_X,
 * and the activation matrix from Unified Buffer is the matrix Y, starting at address address_Y */
void Controller::MatrixMultiply(int A, int B, int C, bool is_dimension_nchw, int channel,
                                unsigned int address_X, unsigned int address_Y) {
    
    int tile_width, tile_height, total_width, total_height, order, multiply_factor;

    int a = int( (A - 1) / systolic_array_height ) + 1; // smallest integer greater than or equal to A / systolic_array_height
    int b = int( (B - 1) / systolic_array_width ) + 1;
    int c = int( (C - 1) / accumulator_size ) + 1;
    int remainder_a = A - ((a - 1) * systolic_array_height);
    int remainder_b = B - ((b - 1) * systolic_array_width);
    int remainder_c = C - ((c - 1) * accumulator_size);

    total_width = B;
    total_height = A;
    multiply_factor = (is_dimension_nchw) ? 1 : channel;    // if dimension is not nchw, it is nhwc -> multiply_factor = channel

    // push tiles: A by B
    for (int aa = 0; aa < a; ++aa) {
        for (int cc = 0; cc < c; ++cc) {
            for (int bb = 0; bb < b; ++bb) {
                // weights
                starting_address = address_X + multiply_factor * ((aa * systolic_array_height * B) + (bb * systolic_array_width));
                tile_width = (bb == (b - 1)) ? remainder_b : systolic_array_width;
                tile_height = (aa == (a - 1)) ? remainder_a : systolic_array_height;

                weight_tile_queue->push_back(MakeTile(id, starting_address, tile_width, tile_height, total_width, total_height));
                // activations
                starting_address = address_Y + multiply_factor * ((bb * systolic_array_width * C) + (cc * accumulator_size));
                tile_width = (cc == (c - 1) ? remainder_c : accumulator_size;
                tile_height = (aa == (a - 1)) ? remainder_a : systolic_array_height;    // same as weight, but just in case

                activation_tile_queue->push_back(MakeTile(id, starting_address, tile_width, tile_height, total_width, total_height));
                // increment id
                id++;
            }
        }
    }
}
