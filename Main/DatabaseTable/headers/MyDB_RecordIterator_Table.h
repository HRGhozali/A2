#ifndef REC_ITER_TABLE_H
#define REC_ITER_TABLE_H

#include <memory>
#include "MyDB_RecordIterator.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_Record.h"

using namespace std;

class MyDB_TableReaderWriter;

class MyDB_RecordIterator_Table : public MyDB_RecordIterator {
public:
    // constructor
    MyDB_RecordIterator_Table(MyDB_TableReaderWriter &parent, MyDB_RecordPtr iterateIntoMe);

    // Overwritten virtual functions
    void getNext() override;
    bool hasNext() override;

private:
    // Parent table reader/writer
    MyDB_TableReaderWriter& parent;

    int currentPage; // The index of the page we are currently iterating over.

    // Pointer to iterate things into
    MyDB_RecordPtr iterateIntoMe;

    MyDB_RecordIteratorPtr pageIterator; // The active iterator for the currenPage.
};

#endif