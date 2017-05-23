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
    slot[slot_id].length = 0;
    memcpy(data + slot[slot_id].offset, &redirect_tid, sizeof(TID));
}

uint16_t SlottedPage::get_length(uint16_t slot_id) {
    return slot[slot_id].length;
}

char *SlottedPage::getData(uint16_t slot_id) {
    return data + slot[slot_id].offset;
}

uint16_t SlottedPage::getOffset(uint16_t slot_id){
    return slot[slot_id].offset;
}

bool SlottedPage::isFree(int len) {
    len = std::max(len, (int) sizeof(TID));
    len += SLOT_SIZE;
    int right = header.dataStart;
    int left = sizeof(Slot) * header.slotCount;
    return (right - left >= len);
}

void SlottedPage::remove(uint16_t slot_id) {
    slot[slot_id].offset = 0;
    slot[slot_id].length = 0;
}

uint16_t SlottedPage::store(uint32_t len, const char *dataa) {
    slot[header.slotCount].length = len;
    len = std::max(len, (uint32_t)sizeof(TID));
    header.dataStart -= len;
    slot[header.slotCount].offset = header.dataStart;
    memcpy(this->data + slot[header.slotCount].offset, dataa, slot[header.slotCount].length);
    header.slotCount++;
    return header.slotCount - 1;
}

void SlottedPage::store(uint32_t len, const char *dataa, uint16_t slot_id) {
    slot[slot_id].length = len;
    len = std::max(len, (uint32_t)sizeof(TID));
    header.dataStart -= len;
    slot[slot_id].offset = header.dataStart;
    memcpy(this->data + slot[slot_id].offset, dataa, slot[slot_id].length);
}

