/** @file
* Created by Toghrul on 27.04.17.
*/
#include "operations.hpp"

using namespace std;

int64_t Operations::getFileSize(std::string fileName) {
    std::ifstream mySource;
    mySource.open(fileName, ios_base::binary);
    mySource.seekg(0, ios_base::end);
    int64_t size = mySource.tellg();
    mySource.close();
    return size;
}

bool Operations::writeToFile(std::string fileName, std::ios_base::openmode base, std::string data) {
    std::ofstream file(fileName, base);

    if(file.is_open()) {
        file << data;
        file.close();
        return true;
    } else {
        std::cout << "unable to open file" << std::endl;
        return false;
    }
}

std::string Operations::readFile(std::string fileName) {
    std::string txt;
    std::ifstream file(fileName);

    if(file.is_open()) {
        while(file.good()) {
            std::getline(file, txt);
        }
    }
    file.close();

    std::cout << txt << std::endl;

    return txt;
}

bool Operations::writeInt64(std::string fileName, std::ios_base::openmode base, std::vector<int64_t> data) {

    throw "writeInt64 is not implemented yet. You can implement it :)";
}


