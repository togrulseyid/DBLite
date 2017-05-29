//
// Created by mahammad on 5/29/17.
//

#ifndef DBLITE_SCHEMASEGMENT_H
#define DBLITE_SCHEMASEGMENT_H

#include <buffer/buffermanager.h>
#include <slottedpages/schema/Schema.hpp>
#include <bits/unique_ptr.h>
#include "segment.h"

class SchemaSegment : Segment{
private:
    Schema* schema;
public:
    SchemaSegment(uint16_t segment_id, BufferManager& bm);
    SchemaSegment(uint16_t segment_id, BufferManager& bm, Schema* schema);
    void write_disk();
    void load_memory();
    Schema* getSchema() {
        return (schema);
    }
};

#endif //DBLITE_SCHEMASEGMENT_H
