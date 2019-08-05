#include "controller.hpp"

Controller::Controller(MatrixMultiplyUnit *matrixmultiplyunit, std::vector<Interconnect *> *icnt_list,
                       std::vector<tile> *weighttilequeue, std::vector<tile> *activationtilequeue) {
    systolic_array_width = matrixmultiplyunit->GetSystolicArrayWidth();
    systolic_array_height = matrixmultiplyunit->GetSystolicArrayHeight();
    accumulator_size = matrixmultiplyunit->GetAccumulatorSize();
    mmu = matrixmultiplyunit;

    id = 1;
    
    interconnect_list = icnt_list;
    std::vector<Interconnect *>::iterator it;
    Interconnect *ptr;
    for (it = interconnect_list->begin(); it != interconnect_list->end(); ++it) {
        ptr = *it;
        ptr->SetController(this);
    }

    weight_tile_queue = weighttilequeue;
    activation_tile_queue = activationtilequeue;
}

/* This function is called in Interconnect::Cycle() when its receiver is not Matrix Multiply Unit
 * In here we look for the Interconnect whose sender is done_unit and if that Interconnect's receiver
 * is Matrix Multiply Unit, delete the request in that Interconnect's waiting_queue with the given order */
void Controller::RaiseDoneSignal(Unit *done_unit, int order) {
    std::vector<Interconnect *>::iterator it;
    Interconnect *ptr;
    for (it = interconnect_list->begin(); it != interconnect_list->end(); ++it) {
        ptr = *it;
        if ((ptr->GetSender() == done_unit) && (ptr->GetReceiver()->IsMatrixMultiplyUnit()))
            find_and_delete_by_order(*ptr->GetWaitingQueue(), order);
    }
}

/* This function is called in MatrixMultiply()
 * It takes two matrices X and Y (A by B and B by C) and tiles them into
 * systolic_array_width by systolic_array_height and systolic_array_width by accumulator_size matrices respectively
 * Then the tiles of corresponding sizes are pushed into request_queues of Matrix Multiply Unit
 * In this case, the weight matrix from Weight Fetcher is the matrix X, starting at address address_X,
 * and the activation matrix from Unified Buffer is the matrix Y, starting at address address_Y */
void Controller::Tile(int A, int B, int C, bool is_dimension_nchw, int channel,
                                unsigned int address_X, unsigned int address_Y) {
    
    int tile_width, tile_height, total_width, total_height, multiply_factor;
    unsigned int starting_address;

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

                weight_tile_queue->push_back(MakeTile(-id, starting_address, multiply_factor, tile_width, tile_height, total_width, total_height));
                // activations
                starting_address = address_Y + multiply_factor * ((bb * systolic_array_width * C) + (cc * accumulator_size));
                tile_width = (cc == (c - 1)) ? remainder_c : accumulator_size;
                tile_height = (aa == (a - 1)) ? remainder_a : systolic_array_height;    // same as weight, but just in case

                activation_tile_queue->push_back(MakeTile(id, starting_address, multiply_factor, tile_width, tile_height, total_width, total_height));
                // increment id
                id++;
            }
        }
    }
}

/* This function is called in MatrixMultiply()
 * It checks tile informations of weight_tile_queue and activation_tile_queue and pushes them into
 * Matrix Multiply Unit's wf_request_queue and ub_request queue, respectively */
void Controller::PushRequestsFromTiles() {
    std::vector<tile>::iterator it;
    std::vector<request> *wf_request_queue = mmu->GetWFRequestQueue();
    std::vector<request> *ub_request_queue = mmu->GetUBRequestQueue();
    float size;

    for (it = weight_tile_queue->begin(); it != weight_tile_queue->end(); ++it) {
        size = (float)(it->tile_width) * (float)(it->tile_height);
        wf_request_queue->push_back(MakeRequest(it->order, size));
    }

    for (it = activation_tile_queue->begin(); it != activation_tile_queue->end(); ++it) {
        size = (float)(it->tile_width) * (float)(it->tile_height);
        ub_request_queue->push_back(MakeRequest(it->order, size));
    }
}

/* This function takes the specified matrix multiplication of weight and activation, and tiles them accordingly via Tile()
 * Afterwards, the tile information in weight_tile_queue and activation_tile_queue are pushed into
 * the connected Matrix Multiplication Unit's wf_request_queue and ub_request_queue, respectively 
 * Note that the two queues are from DRAM and CPU originally, and the tile_queues are not deleted afterwards 
 * since DRAM and possibly CPU will use them to check for address information */
void Controller::MatrixMultiply(int A, int B, int C, bool is_dimension_nchw, int channel,
                                unsigned int address_X, unsigned int address_Y) {
    // first push tiles into tile_queues
    Tile(A, B, C, is_dimension_nchw, channel, address_X, address_Y);
    PushRequestsFromTiles();
}

void Controller::PrintAllTiles() {
    std::cout << "total number of tiles - weights: " << weight_tile_queue->size()
              << ", activations: " << activation_tile_queue->size() << std::endl;
    std::vector<tile>::iterator wit = weight_tile_queue->begin();
    std::vector<tile>::iterator ait = activation_tile_queue->begin();
    while ((wit != weight_tile_queue->end()) || (ait !=activation_tile_queue->end())) {
        if (wit != weight_tile_queue->end()) {
            std::cout << "weight id: " << wit->order << ", address starts at: " << wit->starting_address << std::endl;
            wit++;
        }
        if (ait != activation_tile_queue->end()) {
            std::cout << "activation id: " << ait->order << ", address starts at: " << ait->starting_address << std::endl;
            ait++;
        }
    }
}



