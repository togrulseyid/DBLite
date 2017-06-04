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

const uint16_t HEADER_SIZE = sizeof(SlottedPageHeader); // the size of header part
const uint16_t DATA_SIZE = FRAME_SIZE - HEADER_SIZE; // the size of the data
const uint16_t SLOT_SIZE = sizeof(Slot); // the size of slots


class SlottedPage {
private:
    SlottedPageHeader header; // header of the page
    union { // slots and data are stored in this union
        Slot slots[DATA_SIZE / SLOT_SIZE];
        char data[DATA_SIZE];
    };
public:
    void remove(uint16_t slot_id); // remove the slots with slot_id

    uint16_t store(uint32_t len, const char *data);  // stores a new data in the page

    void store(uint32_t len, const char *data, uint16_t slot_id); // changes the data on the slots

    bool is_free(int len); // check page contains data with the length of len

    void redirect(uint16_t slot_id, TID redirect_tid); // putting tid on the slots data, which means it is quite big

    uint16_t get_length(uint16_t slot_id); // the length of the data on the slots

    uint16_t get_offset(uint16_t slot_id); // the offset of the data on the slots

    char* get_data(uint16_t slot_id); // return the data on the slots

    SlottedPage();
};


#endif //DBLITE_SLOTTEDPAGES_H
