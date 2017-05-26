#include "e_sort/sort.h"
#include <algorithm>
#include <fcntl.h>

void ExternalSort::externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {

    uint64_t numCount = memSize / sizeof(uint64_t); // # 64 bit unsigned integer can be resided on RAM

    if (size % numCount == 0) // k - how many files need to be used to store sorted values.
        k = size / numCount;
    else
        k = size / numCount + 1;
    sizeFiles = new uint64_t[k];
    for (int i = 1; i < k; ++i) {
        sizeFiles[i - 1] = numCount;
    }

    std::cout << "k: " << k << std::endl;

    uint64_t remaining = numCount * (k - 1);
    sizeFiles[k - 1] = size - remaining;
 
    // Here I separate the input file to a few pieces and each piece contains RAM size 64 bit unsigned integer
    // for each set of numbers(k sets) I sort and create a new file for each, labeled from 1 to k

    for (int i = 1; i <= k; ++i) {
        std::string fileNamestr = std::to_string(i);
        char *fileName = new char[fileNamestr.size() + 1];
        std::strcpy(fileName, fileNamestr.c_str());
        uint64_t readSize = sizeFiles[i - 1];
        uint64_t *buffer = new uint64_t[readSize];
        read(fdInput, buffer, readSize * sizeof(uint64_t));

        std::sort(buffer, buffer + readSize);

        int fdout = open(fileName, O_RDWR | O_CREAT, 0666);
        write(fdout, buffer, readSize * sizeof(uint64_t));
        close(fdout);
        delete[] fileName;
        delete[] buffer;
    }
    std::cout << "Merge Phase starts" << std::endl;
    mergePhase(fdOutput, numCount); // to merge all the sorted values stored in the files
    return;
}

void ExternalSort::mergePhase(int fdOutput, uint64_t numCount) {
    for (int i = 1; i <= k; ++i) { // to open all the files in which stored sorted values
        std::string fileNamestr = std::to_string(i);
        char *fileName = new char[fileNamestr.size() + 1];
        std::strcpy(fileName, fileNamestr.c_str());
        fdTemp.push_back(open(fileName, O_RDWR | O_CREAT, 0666));
        delete[] fileName;
    }

    // always to keep this amount values from each file, and the (k+1)th is for buffering the output

    uint64_t size = numCount / (k + 1);

    uint64_t **bufferFiles = new uint64_t *[k]; // keep 'size MB' of each file
    for (int i = 0; i < k; ++i)
        bufferFiles[i] = new uint64_t[size];

    leftCount = new uint64_t[k];
    sizeBuffer = new uint64_t[k];

    for (int i = 0; i < k; ++i) { // here I take size MB values from files to bufferFiles
        uint64_t readSize = std::min(size, sizeFiles[i]);
        leftCount[i] = 0;
        read(fdTemp[i], bufferFiles[i], readSize * sizeof(uint64_t));
        sizeBuffer[i] = readSize;
        pq.push(std::make_pair(bufferFiles[i][0], i));
    }

    uint64_t *buffer = new uint64_t[size]; // output buffer
    uint64_t cnt = 0;

    while (!pq.empty()) { // if pq is empty that means we already processed all values from all files
        std::pair<uint64_t, int> cur = pq.top(); // take the smallest of all files
        pq.pop();
        sizeFiles[cur.second]--;
        leftCount[cur.second]++;
        if (leftCount[cur.second] != sizeBuffer[cur.second]) // we didn't use all numbers from the files resided on RAM
            pq.push(std::make_pair(bufferFiles[cur.second][leftCount[cur.second]], cur.second)); // add the next one from bufferFiles
        buffer[cnt] = cur.first;
        ++cnt;
        if (cnt == size) { // output buffer is full, need to flush into output file
            write(fdOutput, buffer, size * sizeof(uint64_t));
            cnt = 0;
        }
        if (leftCount[cur.second] == sizeBuffer[cur.second]) { // already used all the numbers from RAM for this file
            if (sizeFiles[cur.second] != 0) { // check whether there is any unused numbers from that file
                uint64_t readSize = std::min(size, sizeFiles[cur.second]);
                read(fdTemp[cur.second], bufferFiles[cur.second], readSize * sizeof(uint64_t));
                // read 'size' amount of integers, or less depending on how many numbers left in that file
                leftCount[cur.second] = 0;
                sizeBuffer[cur.second] = readSize;
                pq.push(std::make_pair(bufferFiles[cur.second][0], cur.second));
            }
        }
    }
    if (cnt > 0) { // check if there is anything on the buffer to flush
        write(fdOutput, buffer, cnt * sizeof(uint64_t));
    }
    delete[] bufferFiles;
    delete[] buffer;

    for (int i = 0; i < k; ++i) { // I remove the files that I created for storing the sorted numbers
        std::string fileNamestr = std::to_string(i + 1);
        close(fdTemp[i]);
        remove(fileNamestr.c_str());
    }
    return;
}

void ExternalSort::verifySorted(int fdOutput, uint64_t size, uint64_t memSize) {
    uint64_t cntNums = memSize / sizeof(uint64_t);
    uint64_t cur = 0; // the smallest number for uint64_t

    bool sorted = true;

    uint64_t *buffer = new uint64_t[cntNums]; // buffer to reside at most RAM size numbers
    uint64_t leftNums = size; // how many numbers left for checking

    while (leftNums != 0) {
        uint64_t len = std::min(leftNums, cntNums);
        leftNums -= len;
        read(fdOutput, buffer, len * sizeof(uint64_t)); // takes RAM size of less depending on how many numbers left on the file
        for (int j = 0; j < len; ++j)
            if (buffer[j] < cur) { // if it is sorted, the next value should be greater
                sorted = false;
                break;
            } else
                cur = buffer[j];

    }
    delete[] buffer;

    if (sorted == true) // sorted
        std::cout << "correctly sorted" << std::endl;
    else
        std::cout << "incorrectly sorted" << std::endl;
    return;
}
