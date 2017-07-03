//
// Created by mahammad on 6/29/17.
//

#ifndef DBLITE_REGISTER_H
#define DBLITE_REGISTER_H

#include <slottedpages/schema/Types.hpp>
#include <cstdint>
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <string>
#include <utility>

class Register{
    Types::Tag type; // type of Register

    union{ // value is either int or string
        int value;
        char str[32];
    };

public:
    Register(auto type) : type(type){};

    void set(std::string str); // set the string value

    void set(uint32_t val); // set the integer value

    Types::Tag get_type() const; // return type of the Register

    std::string get_str() const; // return string value

    int get_val() const; // return integer value

    bool operator==(const Register &r) const;
    bool operator<(const Register &r) const;

    size_t get_hash() const{ // calculation of hash
        if(this->get_type() == Types::Tag::Integer)
            return std::hash<uint64_t>() (this->get_val());
        else
            return std::hash<std::string>() (this->get_str());
    }
};

namespace std {
    template<>
    struct hash<Register> {
        size_t operator()(const Register &r) const { // hash function for Register
            return r.get_hash();
        }
    };
};

#endif //DBLITE_REGISTER_H
