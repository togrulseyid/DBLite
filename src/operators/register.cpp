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
        return this->str == r.str;
}

bool Register::operator<(const Register& r) const {
    if(this->type == Types::Tag::Integer)
        return this->value < r.value;
    else
        return this->str < r.str;
}