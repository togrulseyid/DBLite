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

std::vector<std::string> jobs{
        "Driver",
        "Engineer",
        "Worker",
        "Doctor",
        "Manager"
};

std::vector<std::pair<int, int> > test_vec;
std::unordered_map<int, std::string> map_job;

bool check_projection(std::vector<Register> registers) {
    for (std::pair<int, int> ppair : test_vec) {
        if (registers[0].get_val() == ppair.first && registers[1].get_val() == ppair.second)
            return true;
    }
    return false;
}

bool check_hashjoin(std::vector<Register> registers) {
    return map_job[registers[0].get_val()] == registers[3].get_str();
}


int main(int argc, char *argv[]) {

    BufferManager bm1(1000, "table1");
    SPSegment sp_segment1(4, bm1);

    Schema::Relation relation1("table1");

    Schema::Relation::Attribute attributeId;
    attributeId.name = "id";
    attributeId.type = Types::Tag::Integer;
    attributeId.len = sizeof(int32_t);
    relation1.attributes.push_back(attributeId);

    Schema::Relation::Attribute attributeName;
    attributeName.name = "name";
    attributeName.type = Types::Tag::Char;
    attributeName.len = 32;
    relation1.attributes.push_back(attributeName);

    Schema::Relation::Attribute attributeAge;
    attributeAge.name = "age";
    attributeAge.type = Types::Tag::Integer;
    attributeAge.len = sizeof(int32_t);
    relation1.attributes.push_back(attributeAge);

    int len = 3 * 32;
    char *record = new char[len];
    srand(time(NULL));
    int cnt_data = 100;
    for (int i = 1; i <= cnt_data; ++i) {

        int *ptr = reinterpret_cast<int *>(record);
        *ptr = i;

        memset(record + 32, '\0', 32);
        int idx = rand() % names.size();
        strcpy(record + 32, names[idx].c_str());

        ptr = reinterpret_cast<int *>(record + 64);
        *ptr = rand() % 7 + 19;

        test_vec.push_back(std::make_pair(i, *ptr));

        Record rec(len, record);

        sp_segment1.insert(rec);
    }

    TableScan *tablescan = new TableScan(bm1, sp_segment1, relation1);
    tablescan->open();
    Selection *selection = new Selection(*tablescan, 1, "Mahammad");
    Projection *projection = new Projection(*selection, std::vector<int>{0, 2});
    Print print(*projection);
    for(int i = 0; i < 41; ++i)
        std::cout<<"-";
    std::cout<<std::endl;
    std::cout<<relation1.attributes[0].name;
    for(int i = relation1.attributes[0].name.size(); i < 20; ++i)
        std::cout<<" ";
    std::cout<<"|";
    std::cout<<relation1.attributes[2].name;
    for(int i = 20 + relation1.attributes[2].name.size(); i < 39; ++i)
        std::cout<<" ";

    std::cout<<"|"<<std::endl;
    for(int i = 0; i < 41; ++i)
        std::cout<<"-";
    std::cout<<std::endl;

    while (print.next()) {
        std::vector<Register> registers = print.getOutput();
        if (!check_projection(registers)) {
            std::cout << "TableScan, Selection, Projection and Print test unsuccessful!!!" << std::endl;
        }
    }
    for(int i = 0; i < 41; ++i)
        std::cout<<"-";
    std::cout<<std::endl;
    std::cout<<std::endl;

    std::cout << "TableScan, Selection, Projection and Print test successful!" << std::endl;

    std::cout<<std::endl;

    BufferManager bm2(1000, "table2");
    SPSegment sp_segment2(4, bm2);

    Schema::Relation relation2("table2");

    Schema::Relation::Attribute attributeid;
    attributeid.name = "name";
    attributeid.type = Types::Tag::Integer;
    attributeid.len = 32;
    relation2.attributes.push_back(attributeid);

    Schema::Relation::Attribute attributejob;
    attributejob.name = "job";
    attributejob.type = Types::Tag::Char;
    attributejob.len = sizeof(int32_t);
    relation2.attributes.push_back(attributejob);

    len = 2 * 32;
    record = new char[len];
    for (int i = 1; i <= cnt_data; ++i) {
        int* ptr = reinterpret_cast<int*>(record);
        *ptr = i;
        int idx = rand() % jobs.size();
        memset(record + 32, '\0', 32);
        strcpy(record + 32, jobs[idx].c_str());

        map_job.insert({i, jobs[idx]});
        Record rec(len, record);
        sp_segment2.insert(rec);
    }

    for(int i = 0; i < 80; ++i)
        std::cout<<"-";
    std::cout<<"|"<<std::endl;
    std::cout<<relation1.attributes[0].name;
    for(int i = relation1.attributes[0].name.size(); i < 20; ++i)
        std::cout<<" ";
    std::cout<<"|";
    std::cout<<relation1.attributes[1].name;
    for(int i = 20 + relation1.attributes[1].name.size(); i < 39; ++i)
        std::cout<<" ";
    std::cout<<"|";
    std::cout<<relation1.attributes[2].name;
    for(int i = 40 + relation1.attributes[2].name.size(); i < 59; ++i)
        std::cout<<" ";
    std::cout<<"|";
    std::cout<<relation2.attributes[1].name;
    for(int i = 60 + relation2.attributes[1].name.size(); i < 79; ++i)
        std::cout<<" ";
    std::cout<<"|";
    std::cout<<std::endl;
    for(int i = 0; i < 80; ++i)
        std::cout<<"-";
    std::cout<<"|"<<std::endl;


    tablescan->close();
    tablescan->open();

    TableScan *tablescan2 = new TableScan(bm2, sp_segment2, relation2);
    tablescan2->open();

    HashJoin* hashjoin = new HashJoin(*tablescan, *tablescan2, 0, 0);
    hashjoin->open();

    Print print_join(*hashjoin);

    while(print_join.next()){
        std::vector<Register> registers = print_join.getOutput();
        if (!check_hashjoin(registers)) {
            std::cout << "HashJoin test unsuccessful!!!" << std::endl;
        }
    }
    for(int i = 0; i < 80; ++i)
        std::cout<<"-";
    std::cout<<"|"<<std::endl;

    std::cout << "HashJoin test successful!" << std::endl;


}