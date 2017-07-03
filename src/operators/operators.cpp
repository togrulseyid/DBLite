//
// Created by mahammad on 6/27/17.
//
#include <operators/operators.h>
#include <sstream>
#include <iostream>
#include <slottedpages/slottedpages.h>

void TableScan::open(){ // starting from the first page till the end
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
    if(cur_pg_id >= last_pg_id) // check whether we already read all the pages
        return false;

    BufferFrame* bf = &bm.fixPage(cur_pg_id, false);
    SlottedPage* sl_pg = static_cast<SlottedPage*>(bf->getData()); // getting the current row from storage
    registers.clear();

    for(int i = 0; i < (int)attributes.size(); ++i){ // splitting them into the corresponding attributes
        char* data = (sl_pg->get_data(cur_slot_id) + (i * 32));

        Register reg(attributes[i].type);

        if(reg.get_type() == Types::Tag::Integer){
            int* ptr = reinterpret_cast<int*>(data);
            reg.set(*ptr);
        } else{
            std::string s = data;
            reg.set(s);
        }
        registers.push_back(reg); // keeping them on the registers
    }
    bm.unfixPage(*bf, false); // releasing the page
    ++cur_slot_id; // already read one more data(row) from db
    if(sl_pg->get_cnt_slots() == cur_slot_id) // we already read all the data(rows) in this page
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
    std::vector<Register> ret_vector = _operator.getOutput(); // getting the output of the input operator
    registers.clear();

    for(int idx : reg_ids) // projection of the input operator
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
    while(_operator.next()){ // fetching the next data(row) from the input
        registers = _operator.getOutput();
        // check whether the corresponding attribute is matched, when it is integer
        if(registers[idx].get_type() == Types::Tag::Integer && is_int == true && registers[idx].get_val() == value)
            return true;
        // check whether the corresponding attribute is matched, when it is string
        if(registers[idx].get_type() == Types::Tag::Char && is_int == false && registers[idx].get_str() == string_val)
            return true;
    }
    registers.clear();
    return false;
}

void HashJoin::open(){
    while(left.next()){ // adding all the data(rows) from the left to memory(hash map)
        std::vector<Register> registers = left.getOutput();
        hashmap.insert({registers[left_reg_id], registers});
    }
    opened = true;
}

void HashJoin::close() {
    opened = false;
}

std::vector<Register> HashJoin::getOutput() {
    return registers;
}

bool HashJoin::next(){
    while(right.next()){ // fetching data(row) from right
        std::vector<Register> regs = right.getOutput();
        // check whether current row with that id is also on the left table
        std::unordered_map<Register, std::vector<Register> >:: iterator it = hashmap.find(regs[right_reg_id]);
        if(it != hashmap.end()){ // yes, left table also contains
            // combining all the attributes from left and right tables
            registers = it->second;
            for(int i = 0; i < right_reg_id; ++i)
                registers.push_back(regs[i]);
            for(int i = right_reg_id + 1; i < (int)regs.size(); ++i)
                registers.push_back(regs[i]);
            return true;
        }
    }
    return false;
}

void Print::open(){
    opened = true;
}

void Print::close() {
    opened = false;
}

std::vector<Register> Print::getOutput() {
    // printing the current result in a proper way
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
        ss << "|";
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