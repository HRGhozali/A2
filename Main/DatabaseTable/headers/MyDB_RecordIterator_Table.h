#ifndef REC_ITER_TABLE_H
#define REC_ITER_TABLE_H

#include <memory>
#include "MyDB_RecordIterator.h"
#include "MyDB_TableReaderWriter.h"

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
    MyDB_TableReaderWriter& parent;

    size_t currentPos;

    MyDB_RecordPtr iterateIntoMe;
};

#endif