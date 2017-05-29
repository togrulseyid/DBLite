//
// Created by mahammad on 5/29/17.
//

#ifndef DBLITE_SEGMENT_H
#define DBLITE_SEGMENT_H

#include <stdint-gcc.h>
#include <buffer/buffermanager.h>

class Segment {
protected:
    uint16_t segment_id;
    BufferManager &bm;
    uint32_t size;
public:
    Segment(uint16_t segment_id, BufferManager &bm);

    uint16_t get_segment_id();

    uint32_t get_size();
};

#endif //DBLITE_SEGMENT_H
