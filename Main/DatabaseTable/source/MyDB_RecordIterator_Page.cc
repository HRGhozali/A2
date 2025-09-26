#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_RecordIterator_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Page.h"

// Constructor - Initializes the reference in the initializer list (errror fix)
 MyDB_RecordIterator_Page::MyDB_RecordIterator_Page (MyDB_PageReaderWriter& parent, MyDB_RecordPtr iterateIntoMe) :
    parent(parent),
    iterateIntoMe(iterateIntoMe)
{
    // The first record's data begins immediately after the PageHeader
    this->currentPos = sizeof(PageHeader);
}

// Reads the next record from the page into the iterateIntoMe record (assumes it exists)
// this should be called BEFORE the iterator record is first examined
void MyDB_RecordIterator_Page::getNext() {
    // Pointer to the start of the page's raw bytes
    char* pageBytes  = (char*)this->parent.myPage->getBytes(this->parent.myPage);

    // Pointer to the current record's location 
    char* currentRecLoc = pageBytes + this->currentPos;

    // Deserialize the record from this location into the shared record object
    // The fromBinary method returns a pointer to the byte AFTER the record it just read.
    void* nextPosPtr = this->iterateIntoMe->fromBinary(currentRecLoc);

    // Update currPos cursor to the new position by calculating the offset from the page start.
    this->currentPos = (char*) nextPosPtr - pageBytes;
}

// Returns true iff there is another record in the page
// Checks if the currentPos is still within the used data area of the page.
bool MyDB_RecordIterator_Page::hasNext() {
    // Get Page Header
    PageHeader* header= (PageHeader*)this->parent.myPage->getBytes(this->parent.myPage);

    // There is a next record if the current position is before the end of the data end.
    return this->currentPos < (sizeof(PageHeader) + header->endOfData);
}



#endif