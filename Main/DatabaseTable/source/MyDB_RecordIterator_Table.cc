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

    // Get the iterator for the first page.
    this->pageIterator = this->parent[this->currentPage].getIterator(iterateIntoMe);
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
    // If the current page iterator has more records, return true
    cout << "checking for next - table\n";
    if (pageIterator->hasNext()) {
        cout << "page has another record - true\n";
        return true;
    }

    // If not, try to advance to the next page.
    // Keep trying until we find a page with records or run out of pages
    cout << "page has no other record - find another page\n";
    while (1) {
        cout << "moving to new page\n";
        // Move up to the next page
        this->currentPage++;

        // Check if we have run past the last page of the table
        if (this->currentPage >= this->parent.getNumPages()) {
            cout << "no pages left - false\n";
            return false; // No more pages left
        }

        // Get a new page iterator for the new page.
        this->pageIterator = this->parent[this->currentPage].getIterator(iterateIntoMe);

        // Check if this new page has any records
        if (pageIterator->hasNext()) {
            cout << "found non-empty page\n";
            return true; // found a non-empty page
        }
         
    }
}

#endif