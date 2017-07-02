//
// Created by mahammad on 6/27/17.
//
#include <operators/operators.h>

void TableScan::open(){
    cur_pg_id = 0;
    cur_slot_id = 0;
    last_pg_id = sp_segment.get_size();
    opened = true;
}

void TableScan::close() {
    opened = false;
}

std::vector<Register> TableScan::getOutput() {
    return registers;
}

bool TableScan::next(){

}