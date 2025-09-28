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
        cout << "RECORD TABLE DEBUG: Getting first page iterator" << endl;
         // Get the iterator for the first page.
        this->pageIterator = this->parent[this->currentPage].getIterator(iterateIntoMe);
    } else {
        // If the table is empty, store a nullptr.
        cout << "RECORD TABLE DEBUG: Table empty" << endl;
        this->pageIterator = nullptr;
    }

   
}

// put the contents of the next record in the file into the iterator record
// this should be called BEFORE the iterator record is first examined
void MyDB_RecordIterator_Table::getNext() {
    // Rely on hasNext() to have set up the correct page iterator.
    // Delegate the actual work to the underlying page iterator.
    this->pageIterator->getNext();
}

// return true iff there is another record in the file
bool MyDB_RecordIterator_Table::hasNext() {
    // First, handle the case where the table was empty from the start.
    cout << "RECORD TABLE DEBUG: Checking if table has next" << endl;
    if (pageIterator == nullptr) {
        cout << "RECORD TABLE DEBUG: Table empty" << endl;
        return false;
    }
    
    // If the current page iterator has more records, return true
    cout << "RECORD TABLE DEBUG: Checking if page has more records" << endl;
    if (pageIterator->hasNext()) {
        cout << "RECORD TABLE DEBUG: Page has next" << endl;
        return true;
    }

    // If not, try to advance to the next page.
    // Keep trying until we find a page with records or run out of pages
    cout << "RECORD TABLE DEBUG: Page has none - find new page" << endl;
    while (1) {
        cout << "RECORD TABLE DEBUG: Moving to new page" << endl;
        // Move up to the next page
        this->currentPage++;

        // Check if we have run past the last page of the table
        if (this->currentPage >= this->parent.getNumPages()) {
            cout << "RECORD TABLE DEBUG: No pages left - false" << endl;
            return false; // No more pages left
        }

        // Get a new page iterator for the new page.
        this->pageIterator = this->parent[this->currentPage].getIterator(iterateIntoMe);

        // Check if this new page has any records
        if (pageIterator->hasNext()) {
            cout << "RECORD TABLE DEBUG: Found non-empty page" << endl;
            return true; // found a non-empty page
        }
         
    }
}

#endif