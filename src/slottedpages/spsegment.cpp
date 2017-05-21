//
// Created by mahammad on 5/21/17.
//

#include <slottedpages/Record.h>
#include <slottedpages/tid.h>
#include <slottedpages/spsegment.h>
#include <slottedpages/slottedpages.h>
#include <cstring>

TID SPSegment::insert(const Record& r){
    auto recordData = r.getData();
    auto recordSize = r.getLen();

    for(int i = 0; i < size; ++i){
        BufferFrame& frame = bm.fixPage(i, true);
        SlottedPage* page = static_cast<SlottedPage*>(frame.getData());

        if(page->isFree(recordSize)){
            uint16_t slotId = page->store(recordSize, recordData);
            bm.unfixPage(frame, true);
            //TID tid(i, slotId);
            return TID(i, slotId);
        } else{
            bm.unfixPage(frame, false);
        }
    }
    uint16_t pageId = size;
    BufferFrame& frame = bm.fixPage(pageId, true);
    ++size;
    auto data = frame.getData();
    memcpy(data, new SlottedPage(), FRAME_SIZE);
    SlottedPage* page = static_cast<SlottedPage*>(frame.getData());
    uint16_t slotId = page->store(recordSize, recordData);
    bm.unfixPage(frame, true);
    //TID tid(pageId, slotId);
    return TID(pageId, slotId);;
}

bool SPSegment::remove(TID tid){
    uint64_t page_id = tid.getPageId();
    uint32_t slot_id = tid.getSlotId();
    BufferFrame& frame = bm.fixPage(page_id, true);
    SlottedPage* page = static_cast<SlottedPage*>(frame.getData());
    page->remove(slot_id);
    bm.unfixPage(frame, true);
    return true;
}

Record SPSegment::lookup(TID tid){

}

bool SPSegment::update(TID tid, const Record& r){
    uint64_t page_id = tid.getPageId();
    uint32_t slot_id = tid.getSlotId();
    auto recordSize = r.getLen();

    BufferFrame& frame = bm.fixPage(page_id, true);
    SlottedPage* page = static_cast<SlottedPage*>(frame.getData());

    if(page->isFree(recordSize)){
        page->store(recordSize, r.getData(), slot_id);
        bm.unfixPage(frame, true);
        return true;
    }





}

SPSegment::SPSegment(uint16_t segment_id, BufferManager& buffer_manager){
    bm = buffer_manager;
    size = 0;
}