#ifndef TABLE_RW_C
#define TABLE_RW_C

#include "MyDB_RecordIterator_Table.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_Table.h"

MyDB_RecordIterator_Table::MyDB_RecordIterator_Table (MyDB_TableReaderWriter &parent, MyDB_RecordPtr iterateIntoMe) :
    parent(parent),
    iterateIntoMe(iterateIntoMe)
{
    this->currentPos = this->parent.myTable->getStorageLoc();  // idk if this is right
}

// Reads 
void MyDB_RecordIterator_Table::getNext() {
    char* tableBytes = (char*)this->parent.myTable->getStorageLoc().c_str();

    char* currentRecLoc = tableBytes + this->currentPos;

    void* nextPosPtr = this->iterateIntoMe->fromBinary(currentRecLoc);

    this->currentPos = (char*) nextPosPtr - tableBytes;
}

bool MyDB_RecordIterator_Table::hasNext() {
    return this->currentPos < this->parent.myTable->lastPage() * this->parent.pageSize;
}

#endif