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


class SPSegment {
private:
    BufferManager& bm;
    uint32_t size;
    std::priority_queue<std::pair<uint16_t, uint64_t > > free_space_inventory;
public:
    TID insert(const Record &r);

    bool remove(TID tid);

    Record lookup(TID tid);

    bool update(TID tid, const Record &r);

    SPSegment(uint16_t segment_id, BufferManager &buffer_manager);
};

#endif //DBLITE_SPSEGMENT_H
