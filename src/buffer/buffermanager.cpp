//
// Created by mahammad on 5/13/17.
//

#include "buffer/buffermanager.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

BufferManager::BufferManager(unsigned pageCount) {
    this->max_frame_cnt = pageCount;
    this->frame_cnt = 0;
    this->timestamp = 0; // current timestamp is 0, as it is the beginning of BufferManager
    pthread_mutex_init(&lru_lock, NULL);
    pthread_mutex_init(&fr_map_lock, NULL);
}

BufferFrame *BufferManager::readData(uint64_t pageId) {
    BufferFrame *frame;

    int segmentId = pageId >> 40; // in which segment file this page should be resided, each segment file may contains 2^40 pages
    auto find_fd = segment_fd.find(segmentId);
    int fd;

    if (find_fd != segment_fd.end()) { // already opened
        fd = find_fd->second;
    } else { // opening/creating a new segment file
        std::string file = "segment_" + std::to_string(segmentId);
        fd = open(file.c_str(), O_RDWR | O_CREAT, 0666);
        if (fd < 0)
            std::cerr << "Segment file can't be created" << std::endl;
        else
            segment_fd.insert(std::make_pair(segmentId, fd));
    }
    frame = new BufferFrame(fd, pageId); // create a new frame
    frames.insert(std::make_pair(pageId, frame)); // add it to the frames map
    return frame;
}

BufferFrame &BufferManager::fixPage(uint64_t pageId, bool exclusive) {
    BufferFrame *frame;
    pthread_mutex_lock(&fr_map_lock); // locking the whole hashmap

    auto find_pageId = frames.find(pageId); // searching for the page on the BufferManager

    if (find_pageId != frames.end()) { // resided in the BufferManager
        frame = find_pageId->second;
        pthread_mutex_lock(&lru_lock);
        auto find_lru = lru_val.find(pageId);
        if (find_lru != lru_val.end()) { // it is in the LRU, should be removed
            lru.erase(lru.find(std::make_pair(find_lru->second, pageId)));
            lru_val.erase(pageId);
        }
        frame->threads_cnt += 1; // the number of threads working on the page is increased by 1
        pthread_mutex_unlock(&lru_lock);
        pthread_mutex_unlock(&fr_map_lock);
        frame->lock(exclusive); // locked "exclusively"
        return *frame;
    } else {
        if (frame_cnt < max_frame_cnt) { // page is not in the BufferManager, but it has free place to reside the page
            pthread_mutex_lock(&lru_lock);
            frame = readData(pageId); // added to the frames map
            frame_cnt++;
            frame->lock(exclusive); // locked "exclusively"
            pthread_mutex_unlock(&lru_lock);
            pthread_mutex_unlock(&fr_map_lock);
            return *frame;
        } else { // there doesn't exist a free place for the current page
            if (lru.empty()) { // no free frame is available and no used frame can be freed
                std::cerr << "BufferManager is full, no place for current frame" << std::endl;
                pthread_mutex_unlock(&fr_map_lock);
                exit (1);
            } else { // some used frames(not currently used) can be freed

                pthread_mutex_lock(&lru_lock);

                auto lru_last = *lru.begin(); // takes the "oldest" frame added to LRU
                uint64_t evictId = lru_last.second;
                lru.erase(lru.begin());
                lru_val.erase(evictId);
                BufferFrame *evictFrame = frames[(evictId)];
                evictFrame->writeBackToDisk();
                frames.erase(evictId); // removed from frames map
                delete evictFrame; // removed from the main memory

                frame = readData(pageId); // getting new page
                frames.insert(std::make_pair(pageId, frame)); // added to frames map
                pthread_mutex_unlock(&lru_lock);
                pthread_mutex_unlock(&fr_map_lock);
                frame->lock(exclusive); // locked "exclusively"

                return *frame;
            }
        }
    }
}

void BufferManager::unfixPage(BufferFrame &frame, bool isDirty) {
    frame.unlock();
    if (isDirty) // check whether Dirty
        frame.setDirty();

    pthread_mutex_lock(&lru_lock);
    frame.threads_cnt--; // current thread won't use it, that's why the number of threads using the frames is decreased
    if (frame.threads_cnt == 0) { // if no threads use the frame, then it can be exchanged with other frames
        timestamp++; // current time
        lru_val.insert(std::make_pair(frame.getPageId(), timestamp)); // the timestamp of the page is current time
        lru.insert(std::make_pair(timestamp, frame.getPageId())); // added to the end of the set
    }
    pthread_mutex_unlock(&lru_lock);
}

BufferManager::~BufferManager() { //
    pthread_mutex_lock(&fr_map_lock);

    for (std::pair<uint64_t, BufferFrame *> frame : frames) // clearing all the frames from main memory
        delete frame.second;


    for (std::pair<uint64_t, int> fd : segment_fd) // closing all the segment files
        close(fd.second);

    lru.clear(); // clearing LRU
    lru_val.clear(); // clearing LRU
    frames.clear(); // clearing frames hashmap
    segment_fd.clear(); // clearing file descriptors
    pthread_mutex_destroy(&lru_lock);
    pthread_mutex_destroy(&fr_map_lock);
    //system("exec sudo rm -r segment*");
}