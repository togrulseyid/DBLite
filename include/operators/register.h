//
// Created by mahammad on 6/29/17.
//

#ifndef DBLITE_REGISTER_H
#define DBLITE_REGISTER_H

#include <slottedpages/schema/Types.hpp>

class Register{
    Types::Tag type;

    union{
        int value;
        char str[32];
    };

public:
    Register(auto type) : type(type){};

    void set(std::string str);

    void set(uint32_t val);

    Types::Tag get_type();

    std::string get_str();

    int get_val();

    bool operator==(const Register &r);

    bool operator<(const Register &r);
};

#endif //DBLITE_REGISTER_H
