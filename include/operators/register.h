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
    Types::Tag type;

    union{
        int value;
        char str[32];
    };

public:
    Register(auto type) : type(type){};

    void set(std::string str);

    void set(uint32_t val);

    Types::Tag get_type() const;

    std::string get_str() const;

    int get_val() const;

    bool operator==(const Register &r) const;
    bool operator<(const Register &r) const;

    size_t get_hash() const{
        if(this->get_type() == Types::Tag::Integer)
            return std::hash<uint64_t>() (this->get_val());
        else
            return std::hash<std::string>() (this->get_str());
    }
};

namespace std {
    template<>
    struct hash<Register> {
        size_t operator()(const Register &r) const {
            //return (hash<uint64_t>()(v.get_val()) ^ (hash<uint16_t>()(v.slot_id)));
            return r.get_hash();
        }
    };
};

#endif //DBLITE_REGISTER_H
