#ifndef REC_ITER_PAGE_C
#define REC_ITER_PAGE_C

#include "MyDB_PageHeader.h"
#include "MyDB_RecordIterator_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Page.h"
#include "sstream"

// Constructor - Initializes the reference in the initializer list (errror fix)
 MyDB_RecordIterator_Page::MyDB_RecordIterator_Page (MyDB_PageReaderWriter& parent, MyDB_PageHandle iterateOverPage, MyDB_RecordPtr iterateIntoMe) :
    parent(parent),
    iteratePage(iterateOverPage),
    iterateIntoMe(iterateIntoMe)
{
    // The first record's data begins immediately after the PageHeader
    this->currentPos = sizeof(PageHeader);
}

// Reads the next record from the page into the iterateIntoMe record (assumes it exists)
// this should be called BEFORE the iterator record is first examined
void MyDB_RecordIterator_Page::getNext() {
    // Get raw bytes
    void* rawBytes = this->iteratePage->getBytes(); // currpos
    
    // calc size of page + header size
    // frombinary
    // subtract rawbytes from above
    if (rawBytes == nullptr) {
        throw runtime_error("Error: getNext() called on a page with no buffer.");
    }

    // Cast raw bytes to char*
    char* pageBytes = (char*)rawBytes;

    // Cast to header
    PageHeader* header = (PageHeader*) pageBytes;

    // Pointer to the current record's location - initial page pointer + cursor location. 
    char* currentRecLoc = pageBytes + currentPos;


    // Deserialize the record from this location. Expect it to return a pointer
    // to the next position (absolute pointer into same page).
    void* nextPosPtr = this->iterateIntoMe->fromBinary(currentRecLoc);

    if (nextPosPtr == nullptr) {
        throw runtime_error("Error: fromBinary returned nullptr.");
    }

    // Update currentPos cursor (absolute offset)
    this->currentPos +=  ((char*)nextPosPtr - currentRecLoc);
    
}

// Returns true iff there is another record in the page
// Checks if the currentPos is still within the used data area of the page.
bool MyDB_RecordIterator_Page::hasNext() {
    // Get the raw pointer, which could be null
    void* rawBytes = this->iteratePage->getBytes();

    // Check for null before using the pointer
    if (rawBytes == nullptr) {
        return false;
    }

    // Get page header
    PageHeader* header = (PageHeader*)rawBytes;

    // There is a next record if the current position is before the end of the data end.
    return this->currentPos < (sizeof(PageHeader) + header->endOfData);
}



#endif