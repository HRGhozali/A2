#ifndef REC_ITER_PAGE_C
#define REC_ITER_PAGE_C

#include "MyDB_PageHeader.h"
#include "MyDB_RecordIterator_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Page.h"
#include "sstream"

// Constructor - Initializes the reference in the initializer list (errror fix)
 MyDB_RecordIterator_Page::MyDB_RecordIterator_Page (MyDB_PageReaderWriter& parent, MyDB_PageHandleBase iterateOverPage, MyDB_RecordPtr iterateIntoMe) :
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
    cout << "RecordIterator_Page getNext()\n";
    void* rawBytes = this->iteratePage.getBytes();
    if (rawBytes == nullptr) {
        throw runtime_error("Error: getNext() called on a page with no buffer.");
    }

    char* pageBytes = (char*)rawBytes;
    // page size (for bounds checks)
    size_t pageSize = this->iteratePage.getParent().getPageSize();
    char* pageEnd = pageBytes + pageSize;

    PageHeader* header = (PageHeader*) pageBytes;

    // Pointer to the current record's location (absolute)
    char* currentRecLoc = pageBytes + currentPos;
    // char* currentRecLoc = pageBytes + currentPos;

    // Basic bounds check: ensure currentRecLoc is inside the page
    // if (currentRecLoc < pageBytes + header->endOfData || currentRecLoc >= pageEnd) {
    //     throw runtime_error("Error: currentRecLoc out of bounds in getNext().");
    // }

    // Deserialize the record from this location. Expect it to return a pointer
    // to the next position (absolute pointer into same page).
    void* nextPosPtr = this->iterateIntoMe->fromBinary(currentRecLoc);

    if (nextPosPtr == nullptr) {
        throw runtime_error("Error: fromBinary returned nullptr.");
    }

    char* nextPos = (char*) nextPosPtr;

    // Check nextPos is within page bounds and not before currentRecLoc
    if (nextPos < currentRecLoc || nextPos > pageEnd) {
        // Diagnostic: print values to cerr to help debugging
        std::ostringstream oss;
        oss << "fromBinary returned invalid nextPos: "
            << "currentRecLoc=" << (void*)currentRecLoc
            << " nextPos=" << (void*)nextPos
            << " pageStart=" << (void*)pageBytes
            << " pageEnd=" << (void*)pageEnd
            << " currentPos=" << currentPos;
        throw runtime_error(oss.str());
    }

    // Update currentPos (absolute offset)
    this->currentPos +=  ((size_t *)nextPos - (size_t *)currentRecLoc);
    
}


// void MyDB_RecordIterator_Page::getNext() {
//     // Get the raw pointer to the page's bytes
//     cout << "RECORD PAGE DEBUG: Getting raw bytes" << endl;
//     void* rawBytes = this->iteratePage.getBytes();

//     // If this happens, it means getNext() was called inappropriately.
//     if (rawBytes == nullptr) {
//         throw runtime_error("Error: getNext() called on a page with no buffer.");
//     }
//     cout << "RECORD PAGE DEBUG: Getting page bytes" << endl;
//     char* pageBytes = (char*)rawBytes;

//     // Pointer to the current record's location
//     cout << "RECORD PAGE DEBUG: Getting pointer to current record location" << endl;
//     char* currentRecLoc = pageBytes + currentPos;

//     // Deserialize the record from this location
//     cout << "RECORD PAGE DEBUG: Deserializing record from location" << endl;
//     void* nextPosPtr = this->iterateIntoMe->fromBinary(currentRecLoc);

//     // Update the cursor to the new position
//     cout << "RECORD PAGE DEBUG: Updating cursor" << endl;
//     currentPos = (char*)nextPosPtr - pageBytes;
// }

// Returns true iff there is another record in the page
// Checks if the currentPos is still within the used data area of the page.
bool MyDB_RecordIterator_Page::hasNext() {
    cout << "RecordIterator_Page hasNext()\n";
    // Get the raw pointer, which could be null
    cout << "RECORD PAGE DEBUG: Getting raw pointer" << endl;
    void* rawBytes = this->iteratePage.getBytes();

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