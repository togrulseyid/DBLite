//
// Created by mahammad on 6/27/17.
//
#include <operators/operators.h>
#include <sstream>
#include <iostream>
#include <slottedpages/slottedpages.h>

void TableScan::open(){
    cur_pg_id = 0;
    cur_slot_id = 0;
    last_pg_id = sp_segment.get_size();
    //std::cout<<"last: "<<last_pg_id<<std::endl;
    opened = true;
}

void TableScan::close() {
    opened = false;
}

std::vector<Register> TableScan::getOutput() {
    return registers;
}

bool TableScan::next(){
    //std::cout<<std::endl<<"tapildi"<<std::endl;
    if(cur_pg_id >= last_pg_id)
        return false;

    BufferFrame* bf = &bm.fixPage(cur_pg_id, false);

    SlottedPage* sl_pg = static_cast<SlottedPage*>(bf->getData());

    uint32_t offset = 0;
    registers.clear();
    for(int i = 0; i < (int)attributes.size(); ++i){
        char* data = (sl_pg->get_data(cur_slot_id) + (i * 32));

        Register reg(attributes[i].type);

        if(reg.get_type() == Types::Tag::Integer){
            int* ptr = reinterpret_cast<int*>(data);
            reg.set(*ptr);
        } else{
            std::string s = data;
            reg.set(s);
        }
        registers.push_back(reg);
        //registers.push_back(*reg);
    }
    bm.unfixPage(*bf, false);

    ++cur_slot_id;
    //std::cout<<"slot size "<<cur_slot_id<<" "<<sl_pg->get_cnt_slots()<<" "<<last_pg_id<<std::endl;

    if(sl_pg->get_cnt_slots() == cur_slot_id)
        ++cur_pg_id, cur_slot_id = 0;
    return true;
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
        if(registers[idx].get_type() == Types::Tag::Char && is_int == false && registers[idx].get_str() == string_val)
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
    return true; // implement this
}

void Print::open(){
    opened = true;
}

void Print::close() {
    opened = false;
}

std::vector<Register> Print::getOutput() {
    std::stringstream ss;
    int len = 0;
    for(Register reg : registers){
        len += 20;
        if(reg.get_type() == Types::Tag::Integer)
            ss << reg.get_val();
        else
            ss << reg.get_str();
        for(int i = ss.str().size(); i < len; ++i)
            ss << " ";
    }

    std::cout<< ss.str() << std::endl;

    return registers;
}

bool Print::next(){
    bool ret = _operator.next();

    if(ret == false)
        return false;

    registers = _operator.getOutput();

    return true;
}