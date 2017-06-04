//
// Created by mahammad on 5/21/17.
//

#include <slottedpages/schema/Record.h>
#include <slottedpages/tid.h>
#include <slottedpages/spsegment.h>
#include <slottedpages/slottedpages.h>
#include <cstring>
#include <iostream>
#include <slottedpages/segment.h>

SPSegment::SPSegment(uint16_t segment_id, BufferManager &bm) : Segment(segment_id, bm) {}

TID SPSegment::insert(const Record &r) {
    auto record_data = r.getData();
    auto record_size = r.getLen();

    if (!free_space_inventory.empty()) { // check if it not empty
        std::pair<uint16_t, uint64_t> temp = free_space_inventory.top(); // getting the page which is used previously and contains largest empty space
        if (temp.first >= record_size + SLOT_SIZE) { // check whether it has enough place to reside it
            free_space_inventory.pop();
            free_space_inventory.push(std::make_pair(temp.first - SLOT_SIZE - record_size, temp.second));
            BufferFrame &frame = bm.fixPage(temp.second, true); // fixing the page
            SlottedPage *page = static_cast<SlottedPage *>(frame.getData());
            uint32_t slotId = page->store(record_size, record_data); // storing
            bm.unfixPage(frame, true);
            TID tid = {(uint64_t) temp.second, slotId};
            return tid;
        }
    }
    // if it can't fit the page with largest empty place
    uint16_t pageId = size; // taking new page with the smallest id
    BufferFrame &frame = bm.fixPage(pageId, true);
    ++size;
    auto data = frame.getData();
    memcpy(data, new SlottedPage(), FRAME_SIZE);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());
    uint16_t slotId = page->store(record_size, record_data);
    free_space_inventory.push(std::make_pair(DATA_SIZE - SLOT_SIZE - record_size, pageId)); // adding it to the free space inventory to use that page for the further records
    bm.unfixPage(frame, true);
    TID tid = {pageId, slotId};
    return tid;
}

bool SPSegment::remove(TID tid) {
    uint64_t page_id = tid.page_id;
    uint16_t slot_id = tid.slot_id;
    BufferFrame &frame = bm.fixPage(page_id, true);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());
    page->remove(slot_id); // removing the record
    bm.unfixPage(frame, true);
    return true;
}

Record SPSegment::lookup(TID tid) {
    uint64_t page_id = tid.page_id;
    uint16_t slot_id = tid.slot_id;
    BufferFrame &frame = bm.fixPage(page_id, false);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());

    if (page->get_length(slot_id) == 0 && page->get_offset(slot_id) != 0) { // redirected with TID
        TID *redirected_tid = reinterpret_cast<TID *>(page->get_data(slot_id));
        BufferFrame &redirected_frame = bm.fixPage(redirected_tid->page_id, false);
        SlottedPage *redirected_page = static_cast<SlottedPage *>(redirected_frame.getData());
        Record record(redirected_page->get_length(redirected_tid->slot_id),
                      redirected_page->get_data(redirected_tid->slot_id));
        bm.unfixPage(frame, false);
        bm.unfixPage(redirected_frame, false);
        return record;
    } else { // it is not redirected, just fetch it
        Record record(page->get_length(slot_id), page->get_data(slot_id));
        bm.unfixPage(frame, false);
        return record;
    }
}

bool SPSegment::update(TID tid, const Record &r) {
    uint64_t page_id = tid.page_id;
    uint16_t slot_id = tid.slot_id;
    auto recordSize = r.getLen();

    BufferFrame &frame = bm.fixPage(page_id, true);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());

    if (page->is_free(recordSize)) { // there is enough place on the page where the old record resided
        page->store(recordSize, r.getData(), slot_id);
        bm.unfixPage(frame, true);
        return true;
    }
    if(page->get_length(slot_id) == 0){
        TID *old_redirected_tid = reinterpret_cast<TID *>(page->get_data(slot_id));
        BufferFrame &old_redirected_frame = bm.fixPage(old_redirected_tid->page_id, true);
        SlottedPage *old_redirected_page = static_cast<SlottedPage *>(old_redirected_frame.getData());
        old_redirected_page->remove(old_redirected_tid->slot_id);
        bm.unfixPage(old_redirected_frame, true);
    }
    TID redirect_tid = insert(r); // we insert the record to a different page

    page->redirect(slot_id, redirect_tid); // redirect to that page
    bm.unfixPage(frame, true);
    return true;
}