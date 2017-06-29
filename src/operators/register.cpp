//
// Created by mahammad on 6/29/17.
//

#include <operators/register.h>
#include <slottedpages/schema/Types.hpp>

void Register::set(std::string str) {
    // fill
    memset(this->str, '\0', 32);
    strcpy(this->str, str.c_str());
}

void Register::set(uint32_t val) {
    value = val;
}

Types::Tag Register::get_type() {
    return type;
}

std::string Register::get_str(){
    std::string ret = str;
    return ret;
}

uint32_t Register::get_val() {
    return value;
}

bool Register::operator==(const Register &r) {
    if (type == Types::Tag::Integer)
        return value == r.value;
    else
        return str == r.str;
}

bool Register::operator<(const Register &r) {
    if(type == Types::Tag::Integer)
        return value < r.value;
    else
        return str < r.str;
}