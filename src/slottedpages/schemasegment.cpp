//
// Created by mahammad on 5/29/17.
//

#include <cstdint>
#include <buffer/buffermanager.h>
#include <slottedpages/schemasegment.h>

SchemaSegment::SchemaSegment(uint16_t segment_id, BufferManager &bm) : Segment(segment_id, bm) {
    schema = new Schema();
    load_memory();
}

SchemaSegment::SchemaSegment(uint16_t segment_id, BufferManager &bm, Schema* schema) : Segment(segment_id, bm),
                                                                                       schema(schema) {
    write_disk();
}

void SchemaSegment::write_disk() { // writing the schema to the BufferManager/disk as dirty
    BufferFrame& bf = bm.fixPage(0, true);
    char* data = static_cast<char*>(bf.getData());
    memcpy(data, schema, FRAME_SIZE);
    bm.unfixPage(bf, true);
}

void SchemaSegment::load_memory() { // loading the schema from the BufferManager/disk
    BufferFrame& bf = bm.fixPage(0, true);
    char* bf_data = static_cast<char*>(bf.getData());
    schema = (reinterpret_cast<Schema*>(bf_data));
    bm.unfixPage(bf, false);
}