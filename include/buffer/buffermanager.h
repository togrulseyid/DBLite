//
// Created by mahammad on 5/13/17.
//

#ifndef DBLITE_BUFFERMANAGER_H
#define DBLITE_BUFFERMANAGER_H

#include <unordered_map>
#include <bits/unordered_map.h>
#include <set>
#include "bufferframe.h"

class BufferManager {
public:
    BufferManager(unsigned pageCount, std::string str = "");

    BufferFrame &fixPage(uint64_t pageId, bool exclusive); // to fix page pageId, with read/write lock

    void unfixPage(BufferFrame &frame, bool isDirty); // unfix frame

    BufferFrame *readData(uint64_t pageId); // read data of page with pageId

    ~BufferManager();

private:
    int frame_cnt; // current number of frames in the BufferManager
    int max_frame_cnt; // max possible amount of frame in the BufferManager
    std::unordered_map<uint64_t, BufferFrame *> frames; // keeps all the frames in the BufferManager by mapping pageId to frame
    std::unordered_map<uint64_t, int> segment_fd;
    std::unordered_map<uint64_t, uint64_t> lru_val; // maps from pageId to timestamp on LRU
    std::set<std::pair<uint64_t, uint64_t>> lru; // LRU, to find the oldest added page
    uint64_t timestamp; // current time for LRU
    std::string name;
    pthread_mutex_t lru_lock;
    pthread_mutex_t fr_map_lock;
};


#endif //DBLITE_BUFFERMANAGER_H
