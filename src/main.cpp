/** @file
* Created by Toghrul on 27.04.17.
*/
#include <dblite.h>

void printInfo() {
    std::cout << "DBLite started!" << std::endl;
    std::cout << "Version: " << VERSION << std::endl;
}

void printHelp() {
    printf("Usage:\n"
                   "\tOptions:\n"
                   "\t\t-i --info            info about version and DB System\n"
                   "\t\t-h --help         Print this screen\n");
}


int main(int argc, char *argv[]) {

    printInfo();

    //TODO: Allow for concatenation of one char options (e.g. -pdl)
    for(int i = 1; i < argc; i++) {

        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp();
        }

        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0) {
            printInfo();
        }
    }

    return 0;
}