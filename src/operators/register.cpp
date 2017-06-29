//
// Created by mahammad on 6/29/17.
//

#include <operators/register.h>

void Register::set(auto val) {
    if (type == Types::Tag::Integer)
        value = val;
    else
        str = val;
}

Types::Tag get() {
    return type;
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