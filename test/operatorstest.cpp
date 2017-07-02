//
// Created by mahammad on 6/29/17.
//
#include <string>
#include <vector>
#include <slottedpages/spsegment.h>


std::vector<std::string> names{
        "Mahammad",
        "Toghrul",
        "Alakbar",
        "Tahir",
        "Musaqil",
        "Fariz",
        "Gudi",
        "Ilkin",
        "Rauf"
};

int main(int argc, char* argv[]){

    BufferManager bm(1000, "operator");
    SPSegment spsegment(4, bm);



}