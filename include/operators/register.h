//
// Created by mahammad on 6/29/17.
//

#ifndef DBLITE_REGISTER_H
#define DBLITE_REGISTER_H

class Register{
    Types::Tag type;

    union{
        uint32_t value;
        string str;
    };

public:
    Register(auto type) : type(type);

    void set(auto val);

    Types::Tag get();

    bool operator==(const Register &r);

    bool operator<(const Register &r);
};

#endif //DBLITE_REGISTER_H
