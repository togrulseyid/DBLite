//
// Created by mahammad on 5/21/17.
//

#include <slottedpages/slottedpages.h>
#include <cstring>
#include <algorithm>

SlottedPage::SlottedPage() {
    this->header.dataStart = FRAME_SIZE;
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

bool SlottedPage::isFree(uint16_t len) {
    len = std::max(len, sizeof(TID));
    uint32_t right = header.dataStart;
    uint32_t left = sizeof(Slot) * (header.slotCount + 1);
    return ((right - left) >= len);
}

void SlottedPage::remove(uint16_t slot_id) {
    slot[slot_id].offset = 0;
    slot[slot_id].length = 0;
}