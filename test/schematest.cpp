#include <iostream>
#include <memory>
#include <slottedpages/schema/Schema.hpp>
#include <slottedpages/schema/Parser.hpp>
#include <slottedpages/schemasegment.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
        return -1;
    }

    try {
        BufferManager bm(1000);

        Schema* schema = Parser(argv[1]).parse();
        SchemaSegment schemaSegment(0, bm, schema);
        SchemaSegment schemaSegment2(0, bm);
        cout<< schemaSegment.getSchema()->toString().size()<<" "<<schemaSegment.getSchema()->toString() << endl;
        cout<<"_______________________"<<endl;

        cout<< schemaSegment2.getSchema()->toString().size()<<" "<<schemaSegment2.getSchema()->toString() << endl;
        cout<<"_______________________"<<endl;

        Schema* originalSchema = Parser(argv[1]).parse();
        cout << originalSchema->toString().size()<<" "<<originalSchema->toString() << " " << originalSchema->toString().size() << endl;

        if (originalSchema->toString().compare(schemaSegment2.getSchema()->toString()) == 0) {
            cout << "Test successfull" << endl;
            return 0;
        }
    } catch (ParserError& pe) {
        cerr << "Parser Error: " << pe.what() << endl;
    } catch (exception& e) {
        cerr << e.what() << endl;
    }

    cerr << "Test failed" << endl;
    return 1;
}