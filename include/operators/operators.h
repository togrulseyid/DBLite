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
    Operator();

    virtual void open() = 0;

    virtual bool next() = 0;

    virtual std::vector<Register> getOutput() const = 0;

    virtual void close() = 0;
};

class TableScan : Operator {
    BufferManager &bm;
    SPSegment &sp_segment;
    std::vector<Schema::Relation::Attribute> attributes;
    std::vector<Register> registers;
    uint64_t cur_pg_id;
    uint32_t cur_slot_id;
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

class Print : Operator {
    TableScan &table_scan;
    std::vector<Register> registers;
    bool opened;

public:
    Print(TableScan &table_scan) : table_scan(table_scan) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();

};

class Join : Operator {

    BufferManager &bm;
    SPSegment &sp_segment;
    std::vector<Schema::Relation::Attribute> attributes;
    std::vector<Register> registers;
    uint64_t cur_pg_id;
    uint32_t cur_slot_id;
    uint64_t last_pg_id;
    bool opened;

public:
    Join(BufferManager &bm, SPSegment &sp_segment, Schema::Relation &relation) : bm(bm), sp_segment(sp_segment),
                                                                                 attributes(relation.attributes) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();

};

class Selection : Operator {
    TableScan &table_scan;
    std::vector<Register> registers;
    bool opened;

public:
    Selection(TableScan &table_scan) : table_scan(table_scan) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();
};

class Projection : Operator {
    TableScan &table_scan;
    std::vector<Register> registers;
    bool opened;

public:
    Projection(TableScan &table_scan) : table_scan(table_scan) {
        opened = false;
    };

    void open();

    void close();

    bool next();

    std::vector<Register> getOutput();
};


#endif //DBLITE_OPERATORS_H
