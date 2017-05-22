//
// Created by mahammad on 5/21/17.
//

#ifndef DBLITE_SPSEGMENT_H
#define DBLITE_SPSEGMENT_H

#include <cstdint>
#include <buffer/buffermanager.h>
#include <slottedpages/Record.h>
#include "tid.h"


class SPSegment {
private:
    BufferManager bm = NULL;
    uint32_t size;
public:
    TID insert(const Record &r);

    bool remove(TID tid);

    Record lookup(TID tid);

    bool update(TID tid, const Record &r);

    SPSegment(uint16_t segment_id, BufferManager &buffer_manager);
};

#endif //DBLITE_SPSEGMENT_H
