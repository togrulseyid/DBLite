//
// Created by mahammad on 6/29/17.
//

#include <operators/register.h>
#include <slottedpages/schema/Types.hpp>
#include <iostream>

void Register::set(std::string str) {
    // fill
    memset(this->str, '\0', 32); // string may have less chars, that's why beforehand filled with '\0'
    strcpy(this->str, str.c_str());
}

void Register::set(uint32_t val) {
    value = val;
}

Types::Tag Register::get_type() const {
    return type;
}

std::string Register::get_str() const{
    std::string ret = str;
    return ret;
}

int Register::get_val() const{
    return value;
}

bool Register::operator==(const Register& r) const {

    if (this->type == Types::Tag::Integer)
        return this->value == r.value;
    else
        return this->get_str() == r.get_str();
}

bool Register::operator<(const Register& r) const {
    if(this->type == Types::Tag::Integer)
        return this->value < r.value;
    else
        return this->get_str() < r.get_str();
}