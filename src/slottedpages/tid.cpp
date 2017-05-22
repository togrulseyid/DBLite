//
// Created by mahammad on 5/22/17.
//

#include <cstdint>
#include <slottedpages/tid.h>

TID::TID(uint64_t pageid, uint16_t slotid) : page_id(pageid), slot_id(slotid){};