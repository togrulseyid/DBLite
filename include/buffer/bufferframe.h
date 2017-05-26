//
// Created by mahammad on 5/13/17.
//

#ifndef DBLITE_BUFFERFRAME_H
#define DBLITE_BUFFERFRAME_H


#include <cstdint>
#include <stdio.h>
#include <sys/param.h>

const int FRAME_SIZE = 16 * 1024;

class BufferFrame {
public:
    BufferFrame(int fd, uint64_t pageId);

    ~BufferFrame();

    uint64_t getPageId(); // to get the page ID of the page

    void *getData(); // to get the data on the page

    void lock(bool exclusive); // lock "read/write"

    void unlock(); // release the lock

    void setDirty(); // set that data is Dirty/Modified

    int threads_cnt; // how many threads currently reading/writing

    void writeBackToDisk(); // if Dirty, should be written back to disk before taken out of the BufferManager

    void readData(); // read data from disk

private:
    uint64_t pageId;
    void *data;
    off64_t offset; // offset of the page on the segment
    bool is_dirty = false;
    pthread_rwlock_t latch;
    int fd; // file descriptor of the segment which contains page pageId
};


#endif //DBLITE_BUFFERFRAME_H
