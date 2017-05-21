//
// Created by mahammad on 5/21/17.
//

#ifndef DBLITE_SLOTTEDPAGES_H
#define DBLITE_SLOTTEDPAGES_H

#include <stdint-gcc.h>
#include <buffer/bufferframe.h>

const uint16_t HEADER_SIZE = sizeof(SlottedPageHeader);
const uint16_t DATA_SIZE = FRAME_SIZE - HEADER_SIZE;

struct SlottedPageHeader {
    uint16_t slotCount;
    uint16_t dataStart;
};

struct Slot{
    uint16_t offset;
    uint16_t length;
};

class SlottedPage{
private:
    SlottedPageHeader header;
    union{
        Slot slot[DATA_SIZE/sizeof(Slot)];
        char data[DATA_SIZE];
    };
public:
    void remove(uint16_t slot_id);
    uint16_t store(uint32_t len, const char* data);
    uint16_t store(uint32_t len, const char* data, uint32_t slot_id);

    uint16_t isFree(uint16_t len);
    SlottedPage();
};



#endif //DBLITE_SLOTTEDPAGES_H
