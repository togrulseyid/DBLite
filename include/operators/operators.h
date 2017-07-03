//
// Created by mahammad on 6/27/17.
//

#ifndef DBLITE_OPERATORS_H
#define DBLITE_OPERATORS_H

#include <slottedpages/spsegment.h>
#include <operators/register.h>
#include <slottedpages/schema/Schema.hpp>

class Operator {
public:

    virtual void open() = 0;

    virtual bool next() = 0;

    virtual std::vector<Register> getOutput() = 0;

    virtual void close() = 0;
};

class TableScan : public Operator {
    BufferManager &bm;
    SPSegment &sp_segment;
    std::vector<Schema::Relation::Attribute> attributes;
    std::vector<Register> registers;
    uint64_t cur_pg_id;
    int cur_slot_id;
    uint64_t last_pg_id;
    bool opened;

public:
    TableScan(BufferManager &bm, SPSegment &sp_segment, Schema::Relation &relation) : bm(bm), sp_segment(sp_segment),
                                                                                      attributes(relation.attributes) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();
};

class Print : public Operator {
    Operator &_operator;
    std::vector<Register> registers;
    bool opened;

public:
    Print(Operator &_operator) : _operator(_operator) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();

};

class Join : public Operator {

    Operator &left, &right;
    std::unordered_map<Register, std::vector<Register>> hashmap;
    int left_reg_id;
    int right_reg_id;
    std::vector<Register> registers;
    bool opened;

public:
    Join(Operator &left, Operator &right, int left_reg_id, int right_reg_id) : left(left), right(right),
                                                                                 left_reg_id(left_reg_id),
                                                                                 right_reg_id(right_reg_id) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();

};

class Selection : public Operator {
    Operator &_operator;
    std::vector<Register> registers;
    bool opened;
    uint8_t idx;
    int value;
    std::string string_val;
    bool is_int;

public:
    Selection(Operator &_operator, uint8_t idx, std::string val) : _operator(_operator), idx(idx) {
        string_val = val;
        opened = false;
        is_int = false;
    };

    Selection(Operator &_operator, uint8_t idx, uint32_t val) : _operator(_operator), idx(idx) {
        value = val;
        opened = false;
        is_int = true;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();
};

class Projection : public Operator {
    Operator &_operator;
    std::vector<Register> registers;
    std::vector<int> reg_ids;
    bool opened;

public:
    Projection(Operator &_operator, std::vector<int> reg_ids) : _operator(_operator), reg_ids(reg_ids) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();
};

#endif //DBLITE_OPERATORS_H
