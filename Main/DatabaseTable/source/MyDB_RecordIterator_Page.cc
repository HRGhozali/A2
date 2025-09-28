#ifndef REC_ITER_PAGE_C
#define REC_ITER_PAGE_C

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
    // Get the raw pointer to the page's bytes
    cout << "RECORD PAGE DEBUG: Getting raw bytes" << endl;
    void* rawBytes = this->parent.myPage.getBytes();

    // If this happens, it means getNext() was called inappropriately.
    if (rawBytes == nullptr) {
        throw runtime_error("Error: getNext() called on a page with no buffer.");
    }
    cout << "RECORD PAGE DEBUG: Getting page bytes" << endl;
    char* pageBytes = (char*)rawBytes;

    // Pointer to the current record's location
    cout << "RECORD PAGE DEBUG: Getting pointer to current record location" << endl;
    char* currentRecLoc = pageBytes + currentPos;

    // Deserialize the record from this location
    cout << "RECORD PAGE DEBUG: Deserializing record from location" << endl;
    void* nextPosPtr = this->iterateIntoMe->fromBinary(currentRecLoc);

    // Update the cursor to the new position
    cout << "RECORD PAGE DEBUG: Updating cursor" << endl;
    currentPos = (char*)nextPosPtr - pageBytes;
}

// Returns true iff there is another record in the page
// Checks if the currentPos is still within the used data area of the page.
bool MyDB_RecordIterator_Page::hasNext() {
    // Get the raw pointer, which could be null
    cout << "RECORD PAGE DEBUG: Getting raw pointer" << endl;
    void* rawBytes = this->parent.myPage.getBytes();

    // Check for null before using the pointer
    if (rawBytes == nullptr) {
        cout << "page has no valid memory buffer\n";
        return false;
    }

    cout << "getting page header\n";
    PageHeader* header = (PageHeader*)rawBytes;
    cout << "returning if next\n";
    // There is a next record if the current position is before the end of the data end.
    return this->currentPos < (sizeof(PageHeader) + header->endOfData);
}



#endif