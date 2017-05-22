//
// Created by mahammad on 5/21/17.
//

#ifndef DBLITE_SLOTTEDPAGES_H
#define DBLITE_SLOTTEDPAGES_H

#include <stdint-gcc.h>
#include <buffer/bufferframe.h>
#include "tid.h"


struct SlottedPageHeader {
    uint16_t slotCount;
    uint16_t dataStart;
};

struct Slot {
    uint16_t offset;
    uint16_t length;
};

const uint16_t HEADER_SIZE = sizeof(SlottedPageHeader);
const uint16_t DATA_SIZE = FRAME_SIZE - HEADER_SIZE;
const uint16_t SLOT_SIZE = sizeof(Slot);


class SlottedPage {
private:
    SlottedPageHeader header;
    union {
        Slot slot[DATA_SIZE / SLOT_SIZE];
        char data[DATA_SIZE];
    };
public:
    void remove(uint16_t slot_id);

    uint16_t store(uint32_t len, const char *data);

    void store(uint32_t len, const char *data, uint16_t slot_id);

    bool isFree(int len);

    void redirect(uint16_t slot_id, TID redirect_tid);

    uint16_t get_length(uint16_t slot_id);

    uint16_t getOffset(uint16_t slot_id);

    char* getData(uint16_t slot_id);

    SlottedPage();
};


#endif //DBLITE_SLOTTEDPAGES_H
