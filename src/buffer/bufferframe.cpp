//
// Created by mahammad on 5/13/17.
//

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "buffer/bufferframe.h"
#define page_idx (1LL<<40) // (1LL<<40) - segment size, how many page it can contain


BufferFrame::BufferFrame(int fd, uint64_t pageId) {
    pthread_rwlock_init(&latch, NULL);
    this->threads_cnt = 1; // newly created, the number of threads working on it should be 1
    this->pageId = pageId;
    this->fd = fd;
    this->offset = ((pageId % page_idx)) * frame_size;
    this->readData();
}

void BufferFrame::readData() {
    data = new uint8_t[frame_size];
    int res = pread(fd, data, frame_size, offset);
    if (res < 0)
        std::cerr << "file can't be read" << std::endl;
}

void *BufferFrame::getData() {
    return data;
}

uint64_t BufferFrame::getPageId() {
    return pageId;
}

void BufferFrame::writeBackToDisk() {
    if (is_dirty) {
        int res = pwrite(fd, data, frame_size, offset);
        if (res < 0)
            std::cerr << "data can't be written to disk" << std::endl;
        is_dirty = false;
    }
}

void BufferFrame::setDirty() {
    is_dirty = true;
}

void BufferFrame::lock(bool exclusive) {
    if (exclusive)
        pthread_rwlock_wrlock(&latch);
    else
        pthread_rwlock_rdlock(&latch);
}

void BufferFrame::unlock() {
    pthread_rwlock_unlock(&latch);
}

BufferFrame::~BufferFrame() {
    writeBackToDisk();
    delete[] data;
    pthread_rwlock_destroy(&latch);
}
