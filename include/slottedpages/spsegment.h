//
// Created by mahammad on 5/21/17.
//

#ifndef DBLITE_SPSEGMENT_H
#define DBLITE_SPSEGMENT_H

#include <cstdint>
#include <buffer/buffermanager.h>
#include <slottedpages/schema/Record.h>
#include <queue>
#include "tid.h"
#include "segment.h"


class SPSegment : Segment{
private:
    std::priority_queue<std::pair<uint16_t, uint64_t > > free_space_inventory; // to quickly find the page with enough space
public:
    TID insert(const Record &r); // insert a new record

    bool remove(TID tid); // remove the record with tid

    Record lookup(TID tid); // look up the record with tid

    bool update(TID tid, const Record &r); // update the record with tid with new record

    SPSegment(uint16_t segment_id, BufferManager &buffer_manager);
};

#endif //DBLITE_SPSEGMENT_H
