#ifndef REC_ITER_TABLE_C
#define REC_ITER_TABLE_C

#include "MyDB_RecordIterator_Table.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Table.h"

MyDB_RecordIterator_Table::MyDB_RecordIterator_Table (MyDB_TableReaderWriter &parent, MyDB_RecordPtr iterateIntoMe) :
    parent(parent),
    iterateIntoMe(iterateIntoMe)
{
    // Start iterating from the very first page.
    this->currentPage = 0;

    // Check if the table has any pages before trying to get an iterator.
    if (this->parent.getNumPages() > 0) {
        //cout << "RECORD TABLE DEBUG: Getting first page iterator" << endl;
        // Get the iterator for the first page.
        this->pageIterator = this->parent[this->currentPage].getIterator(iterateIntoMe);
    } else {
        // If the table is empty, store a nullptr.
        //cout << "RECORD TABLE DEBUG: Table empty" << endl;
        this->pageIterator = nullptr;
    }

   
}

// put the contents of the next record in the file into the iterator record
// this should be called BEFORE the iterator record is first examined
void MyDB_RecordIterator_Table::getNext() {
    // Rely on hasNext() to have set up the correct page iterator.
    // Delegate the actual work to the underlying page iterator.
    //cout << "RecordIterator_Table getNext()\n";
    this->pageIterator->getNext();
}

// return true iff there is another record in the file
bool MyDB_RecordIterator_Table::hasNext() {
    //cout << "RecordIterator_Table hasNext()\n";
    // If table empty
    if (!this->pageIterator) {
    //    cout << "RECORD TABLE DEBUG: Table empty" << endl;
        return false;
    }

    // If current page has more records
    if (this->pageIterator->hasNext()) {
    //    cout << "RECORD TABLE DEBUG: Page has next" << endl;
        return true;
    }

    // Otherwise, advance to next non-empty page up to last page
    int lastPageIndex = this->parent.myTable->lastPage(); // direct access to underlying table's lastPage()
    int nextPage = this->currentPage + 1;

    while (nextPage <= lastPageIndex) {
        // move to nextPage
        this->currentPage = nextPage;
    //    cout << "RECORD TABLE DEBUG: Moving to new page" << endl;

        this->pageIterator = this->parent[this->currentPage].getIterator(iterateIntoMe);
        if (this->pageIterator && this->pageIterator->hasNext()) {
    //        cout << "RECORD TABLE DEBUG: Found non-empty page" << endl;
            return true;
        }
        nextPage++;
    }

    return false;
}

#endif