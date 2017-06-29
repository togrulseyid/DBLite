#include <iostream>
#include <memory>
#include <slottedpages/schema/Schema.hpp>
#include <slottedpages/schema/Parser.hpp>
#include <slottedpages/schemasegment.h>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
        return -1;
    }
    try {
        BufferManager bm(100, "schema");

        Schema *schema = Parser(argv[1]).parse();
        SchemaSegment schemaSegment(0, bm, schema); // write down schema to the BM/disk
        SchemaSegment schemaSegment2(0, bm); // getting schema from BM/disk

        Schema *originalSchema = Parser(argv[1]).parse(); // getting schema again from file
        // checking the two schema strings whether they are equal
        if (originalSchema->toString().compare(schemaSegment2.getSchema()->toString()) == 0) {
            cout << "Test successfull" << endl;
            return 0;
        }
    } catch (ParserError &pe) {
        cerr << "Parser Error: " << pe.what() << endl;
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    cerr << "Test failed" << endl;
    return 1;
}