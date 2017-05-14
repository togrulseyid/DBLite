#include <dblite.h>
#include <e_sort/sort.h>
#include <fcntl.h>
#include <operations.hpp>

void externalSort(int argc, char **pString);

int main(int argc, char *argv[]) {

    //TODO: Allow for concatenation of one char options (e.g. -pdl)

    externalSort(argc, argv);

    return 0;
}

bool check(std::string first, char *second) {
    int len = first.size();
    for (int i = 0; i < len; ++i)
        if (first[i] != second[i])
            return false;
    return true;
}

void externalSort(int argc, char **pString) {

    if (argc > 0) {
        Operations ops;
        uint64_t size = ops.getFileSize(pString[2]);

        if (check("sort", pString[1])) {
            std::cout << "External sort started for " << pString[2] << std::endl;
            int fdInput, fdOutput;
            if ((fdInput = open(pString[2], O_RDONLY)) < 0) {
                std::cerr << "cannot open file '" << pString[2] << "': " << strerror(errno) << std::endl;
            }

            fdOutput = open(pString[3], O_RDWR | O_CREAT, 0666);
            uint64_t memSize = atoll(pString[4]);
            std::cout<<"Machine RAM: "<<memSize<<" MB"<<std::endl;
            memSize = memSize * 1024 * 1024;
            ExternalSort externalsort;
            size = size / sizeof(uint64_t);

            externalsort.externalSort(fdInput, size, fdOutput, memSize);
            close(fdOutput);
            fdOutput = open(pString[3], O_RDONLY);
            externalsort.verifySorted(fdOutput, size, memSize);
        }
    }
}