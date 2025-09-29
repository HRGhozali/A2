#ifndef REC_ITER_PAGE_H
#define REC_ITER_PAGE_H

#include <memory>
#include "MyDB_RecordIterator.h"
#include "MyDB_PageHeader.h"
#include "MyDB_PageHandle.h"
#include "MyDB_PageReaderWriter.h"

using namespace std;

// Forward declare the PageReaderWriter to avoid circular includes
class MyDB_PageReaderWriter;

class MyDB_RecordIterator_Page : public MyDB_RecordIterator {

public:

    // Constructor: accepts a reference to its parent and a record to iterate into
    MyDB_RecordIterator_Page(MyDB_PageReaderWriter &parent, MyDB_PageHandle iterateOverPage, MyDB_RecordPtr iterateIntoMe);

    // The overriden virtual functions
    void getNext() override;
    bool hasNext() override;

private:

    // A reference to the object that manages the page we are iterating over
    MyDB_PageReaderWriter& parent;

    // Reference to page that's being iterated over
    MyDB_PageHandle iteratePage;

    // The current byte offset into the page's record data. Defined from the beginning of the first record.
    size_t currentPos;

    // Record to iterate into
    MyDB_RecordPtr iterateIntoMe;
};



#endif
