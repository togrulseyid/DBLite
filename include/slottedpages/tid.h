//
// Created by mahammad on 5/21/17.
//

#ifndef DBLITE_TID_H
#define DBLITE_TID_H

#include <cstdint>

class TID {
private:
    uint64_t page_id;
    uint32_t slot_id;
public:
    TID(uint64_t page_id, uint32_t slot_id) : (this->page_id, this->slot_id);

    uint64_t getPageId() { return page_id; };

    uint32_t getSlotId() { return slot_id; };
};

#endif //DBLITE_TID_H
