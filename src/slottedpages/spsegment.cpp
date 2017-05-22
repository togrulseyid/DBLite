//
// Created by mahammad on 5/21/17.
//

#include <slottedpages/Record.h>
#include <slottedpages/tid.h>
#include <slottedpages/spsegment.h>
#include <slottedpages/slottedpages.h>
#include <cstring>

TID SPSegment::insert(const Record &r) {
    auto recordData = r.getData();
    auto recordSize = r.getLen();

    for (int i = 0; i < size; ++i) {
        BufferFrame &frame = bm.fixPage(i, true);
        SlottedPage *page = static_cast<SlottedPage *>(frame.getData());

        if (page->isFree(recordSize)) {
            uint16_t slotId = page->store(recordSize, recordData);
            bm.unfixPage(frame, true);
            //TID tid(i, slotId);
            return TID(i, slotId);
        } else {
            bm.unfixPage(frame, false);
        }
    }
    uint16_t pageId = size;
    BufferFrame &frame = bm.fixPage(pageId, true);
    ++size;
    auto data = frame.getData();
    memcpy(data, new SlottedPage(), FRAME_SIZE);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());
    uint16_t slotId = page->store(recordSize, recordData);
    bm.unfixPage(frame, true);
    //TID tid(pageId, slotId);
    return TID(pageId, slotId);;
}

bool SPSegment::remove(TID tid) {
    uint64_t page_id = tid.getPageId();
    uint32_t slot_id = tid.getSlotId();
    BufferFrame &frame = bm.fixPage(page_id, true);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());
    page->remove(slot_id);
    bm.unfixPage(frame, true);
    return true;
}

Record SPSegment::lookup(TID tid) {
    uint64_t page_id = tid.getPageId();
    uint32_t slot_id = tid.getSlotId();

    BufferFrame &frame = bm.fixPage(page_id, false);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());

    if (page->get_length(slot_id) == 0 && true) { // redirected
        TID *redirected_tid = static_cast<TID *>(page->getData(slot_id));
        BufferFrame &redirected_frame = bm.fixPage(page_id, false);
        SlottedPage *redirected_page = static_cast<SlottedPage *>(frame.getData());
        Record record(redirected_page->get_length(redirected_tid->getSlotId()),
                      redirected_page->getData(redirected_tid->getSlotId()));
        bm.unfixPage(frame, false);
        bm.unfixPage(redirected_frame, false);
        return record;
    } else {
        Record record(page->get_length(slot_id), page->getData(slot_id));
        bm.unfixPage(frame, false);
        return record;
    }
}

bool SPSegment::update(TID tid, const Record &r) {
    uint64_t page_id = tid.getPageId();
    uint32_t slot_id = tid.getSlotId();
    auto recordSize = r.getLen();

    BufferFrame &frame = bm.fixPage(page_id, true);
    SlottedPage *page = static_cast<SlottedPage *>(frame.getData());

    if (page->isFree(recordSize)) {
        page->store(recordSize, r.getData(), slot_id);
        bm.unfixPage(frame, true);
        return true;
    }

    TID redirect_tid = insert(r);
    page->slot[slot_id].length = 0;
    page->redirect(slot_id, redirect_tid);
    bm.unfixPage(frame, true);
    return true;
}

SPSegment::SPSegment(uint16_t segment_id, BufferManager &buffer_manager) {
    bm = buffer_manager;
    size = 0;
}