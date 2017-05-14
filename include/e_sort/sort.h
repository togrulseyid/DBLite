#ifndef DBLITE_EXTERNAL_SORT_H
#define DBLITE_EXTERNAL_SORT_H

#include <dblite.h>
#include <vector>
#include <queue>


struct compare {
    bool operator()(const std::pair<uint64_t, int> &l, const std::pair<uint64_t, int> &r) {
        return l.first > r.first;
    }
};


class ExternalSort {
public:
    void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize); // sorts big files within limited RAM

    void mergePhase(int fdOutput, uint64_t numCount); // to merge k sorted files

    void verifySorted(int fdOutput, uint64_t size, uint64_t memSize); // this verifies it is sorted

private:
    std::vector<uint64_t> fdTemp;
    uint64_t *sizeFiles; // how many numbers of each file resided on RAM
    uint64_t *leftCount; // how many numbers already flushed to output file
    uint64_t *sizeBuffer; // how many 64 bit unsigned int resided on each file
    uint64_t k; // number of files to store sorted files
    std::priority_queue<std::pair<uint64_t, int>, std::vector<std::pair<uint64_t, int>>, compare> pq;
    // pq is a min-heap, used to find the smallest element of each files during each iteration
    // pq at most may contain k values.
    // first of pair is the value itself, the second specifies to which files it belongs
};


#endif //DBLITE_EXTERNAL_SORT_H
