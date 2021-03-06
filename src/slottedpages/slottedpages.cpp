//
// Created by mahammad on 5/21/17.
//

#include <slottedpages/slottedpages.h>
#include <cstring>
#include <algorithm>
#include <iostream>

SlottedPage::SlottedPage() {
    this->header.dataStart = DATA_SIZE;
    this->header.slotCount = 0;
}

void SlottedPage::redirect(uint16_t slot_id, TID redirect_tid) {
    slots[slot_id].length = 0;
    memcpy(data + slots[slot_id].offset, &redirect_tid, sizeof(TID)); // copy the TID data to the slots
}

uint16_t SlottedPage::get_length(uint16_t slot_id) {
    return slots[slot_id].length;
}

char *SlottedPage::get_data(uint16_t slot_id) {
    return data + slots[slot_id].offset;
}

uint16_t SlottedPage::get_offset(uint16_t slot_id){
    return slots[slot_id].offset;
}

bool SlottedPage::is_free(int len) {
    len = std::max(len, (int) sizeof(TID)); // we should have store at leat TID size place, because we may redirect it
    len += SLOT_SIZE; // it should also add one slots for the new data
    int right = header.dataStart; // the start of the last data
    int left = sizeof(Slot) * header.slotCount; // how many bytes are used by slots
    return (right - left >= len); // if the empty places more than required then we can place it
}

void SlottedPage::remove(uint16_t slot_id) { // remove the data on the slot_id
    slots[slot_id].offset = 0;
    slots[slot_id].length = 0;
}

uint16_t SlottedPage::store(uint32_t len, const char *dataa) {
    slots[header.slotCount].length = len; // setting the length of the new data
    len = std::max(len, (uint32_t)sizeof(TID)); // we should have store at leat TID size place, because we may redirect it
    header.dataStart -= len; // the head of the new data is updated
    slots[header.slotCount].offset = header.dataStart; // offset is set to the head of the new data
    memcpy(this->data + slots[header.slotCount].offset, dataa, slots[header.slotCount].length); // copying the new data to page
    header.slotCount++; // the number of slots is incremented
    return header.slotCount - 1; // return the slots id
}

void SlottedPage::store(uint32_t len, const char *dataa, uint16_t slot_id) {
    slots[slot_id].length = len; // the data changed with the new one
    len = std::max(len, (uint32_t)sizeof(TID));
    header.dataStart -= len;
    slots[slot_id].offset = header.dataStart;
    memcpy(this->data + slots[slot_id].offset, dataa, slots[slot_id].length);
}

