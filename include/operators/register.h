//
// Created by mahammad on 6/29/17.
//

#ifndef DBLITE_REGISTER_H
#define DBLITE_REGISTER_H

class Register{
    Types::Tag type;

    union{
        uint32_t value;
        char str[32];
    };

public:
    Register(auto type) : type(type){};

    void set(string str);

    void set(uint32_t val);

    Types::Tag get_type();

    string get_str();

    uint32_t get_val();

    bool operator==(const Register &r);

    bool operator<(const Register &r);
};

#endif //DBLITE_REGISTER_H
