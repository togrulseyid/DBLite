//
// Created by mahammad on 6/27/17.
//
#include <operators/operators.h>
#include <sstream>
#include <iostream>

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

void Projection::open(){
    opened = true;
}

void Projection::close() {
    opened = false;
}

std::vector<Register> Projection::getOutput() {
    return registers;
}

bool Projection::next(){
    bool ret = _operator.next();
    if(ret == false)
        return false;
    std::vector<Register> ret_vector = _operator.getOutput();
    registers.clear();

    for(int idx : reg_ids)
        registers.push_back(ret_vector[idx]);

    return true;
}

void Selection::open(){
    opened = true;
}

void Selection::close() {
    opened = false;
}

std::vector<Register> Selection::getOutput() {
    return registers;
}

bool Selection::next(){
    while(_operator.next()){
        registers = _operator.getOutput();
        if(registers[idx].get_type() == Types::Tag::Integer && is_int == true && registers[idx].get_val() == value)
            return true;
        if(registers[idx].get_type() == Types::Tag::Char && is_int == true && registers[idx].get_str() == string_val)
            return true;
    }
    registers.clear();
    return false;
}

void Join::open(){
    opened = true;
}

void Join::close() {
    opened = false;
}

std::vector<Register> Join::getOutput() {
    return registers;
}

bool Join::next(){

}

void Print::open(){
    opened = true;
}

void Print::close() {
    opened = false;
}

std::vector<Register> Print::getOutput() {
    return registers;
}

bool Print::next(){
    bool ret = _operator.next();

    if(ret == false)
        return false;

    registers = _operator.getOutput();

    std::stringstream ss;

    for(Register reg : registers){
        if(reg.get_type() == Types::Tag::Integer)
            ss << reg.get_val();
        else
            ss << reg.get_str();
    }

    std::cout<< ss.str() << std::endl;

    return ret;
}