//
// Created by mahammad on 5/29/17.
//

#include <cstdint>
#include <buffer/buffermanager.h>
#include <slottedpages/segment.h>

Segment::Segment(uint16_t segment_id, BufferManager &bm) : segment_id(segment_id), bm(bm) { size = 0;};

uint16_t Segment::get_segment_id() { return segment_id; };

uint32_t Segment::get_size() { return size; };