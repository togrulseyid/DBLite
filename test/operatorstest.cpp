//
// Created by mahammad on 6/29/17.
//
#include <string>
#include <vector>
#include <slottedpages/spsegment.h>
#include <slottedpages/schema/Schema.hpp>
#include <operators/operators.h>
#include <iostream>


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

std::vector<std::pair<int, int> > test_vec;

bool check_projection(std::vector<Register> registers){
    for(std::pair<int, int> ppair : test_vec){
        if(registers[0].get_val() == ppair.first && registers[1].get_val() == ppair.second)
            return true;
    }
    return false;
}

int main(int argc, char* argv[]){

    BufferManager bm(1000, "operator");
    SPSegment sp_segment(4, bm);

    Schema::Relation relation("Relation");

    Schema::Relation::Attribute attributeId;
    attributeId.name = "id";
    attributeId.type = Types::Tag::Integer;
    attributeId.len = sizeof(int32_t);
    relation.attributes.push_back(attributeId);

    Schema::Relation::Attribute attributeName;
    attributeName.name = "name";
    attributeName.type = Types::Tag::Char;
    attributeName.len = 32;
    relation.attributes.push_back(attributeName);

    Schema::Relation::Attribute attributeAge;
    attributeAge.name = "age";
    attributeAge.type = Types::Tag::Integer;
    attributeAge.len = sizeof(int32_t);
    relation.attributes.push_back(attributeAge);

    int len = 3 * 32;
    srand (time(NULL));
    for(int i = 1; i <= 100; ++i){
        char* record = new char[len];

        int* ptr = reinterpret_cast<int*>(record);
        *ptr = i;

        memset(record + 32, '\0', 32);
        int idx = rand() % names.size();
        strcpy(record + 32, names[idx].c_str());

        ptr = reinterpret_cast<int*>(record + 64);
        *ptr = rand() % 7 + 19;

        test_vec.push_back(std::make_pair(i, *ptr));

        Record rec(len, record);

        sp_segment.insert(rec);
    }

    TableScan *tablescan = new TableScan(bm, sp_segment, relation);
    tablescan->open();
    Selection *selection = new Selection(*tablescan, 1, "Mahammad");
    Projection *projection = new Projection(*selection, std::vector<int>{0, 2});
    Print print(*projection);
    while(print.next()){
        std::vector<Register> registers = print.getOutput();
        if(!check_projection(registers)){
            std::cout<<"Selection and Projection test unsuccessful"<<std::endl;
        }
    }
    std::cout<<"Selection and Projection test successful"<<std::endl;



}