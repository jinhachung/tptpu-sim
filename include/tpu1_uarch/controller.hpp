#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "common.hpp"
#include "dram.hpp"
#include "interconnect.hpp"
#include "mmu.hpp"
#include "unit.hpp"

class Interconnect;
class MatrixMultiplyUnit;
struct tile;
typedef struct tile tile;

class Controller {
public:
    Controller(MatrixMultiplyUnit *matrixmultiplyunit, std::vector<Interconnect *> *icnt_list,
               std::vector<tile> *weighttilequeue, std::vector<tile> *activationtilequeue);
    ~Controller() {delete interconnect_list;}
    void RaiseDoneSignal(Unit *done_unit, int order);
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

    int id;                                         // number to set for next request's order in tiling process

    std::vector<Interconnect *> *interconnect_list;   // list of all interconnects in the architecture

    std::vector<tile> *weight_tile_queue;
    std::vector<tile> *activation_tile_queue;
};

#endif
