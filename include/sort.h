//
// Created by toghrul on 27.04.17.
//

#ifndef DBLITE_EXTERNAL_SORT_H
#define DBLITE_EXTERNAL_SORT_H

#include <dblite.h>

class ExternalSort {
public:
    void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);
};


#endif //DBLITE_EXTERNAL_SORT_H
