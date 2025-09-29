
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageHeader.h"
#include "MyDB_RecordIterator_Page.h"

MyDB_PageReaderWriter::MyDB_PageReaderWriter(MyDB_PageHandle pageToManage): myPage(pageToManage) {
    // The initializer list handles everything. The body can be empty.
}

MyDB_PageReaderWriter::MyDB_PageReaderWriter() {

}

// empties out the contents of this page, so that it has no records in it
// the type of the page is set to MyDB_PageType :: RegularPage
void MyDB_PageReaderWriter :: clear () {
	// Get the raw pointer to the page's bytes
    void* rawBytes = this->myPage->getBytes();

    // If this happens, it means clear() was called inappropriately.
    if (rawBytes == nullptr) {
        throw runtime_error("Error: clear() called on a page with no buffer.");
    }

	// Cast rawBytes to a PageHeader.
    PageHeader* header = (PageHeader*)rawBytes;

	// Set endOfData and numRecords to zero. Indicates page is empty.
	header->endOfData = 0;
	header->numRecords = 0;

	// Sets the correct page type
	header->pageType = MyDB_PageType::RegularPage;

	// Mark the page as dirty 
	this->myPage->wroteBytes();

}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	// Get the raw pointer to the page's bytes
    void* rawBytes = this->myPage->getBytes();

    // If this happens, it means clear() was called inappropriately.
    if (rawBytes == nullptr) {
        throw runtime_error("Error: clear() called on a page with no buffer.");
    }

	// Cast rawBytes to a PageHeader.
    PageHeader* header = (PageHeader*)rawBytes;
	return header->pageType;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	// Create a new page iterator, injecting a reference to this MyDB_PageReaderWriter object
	// and the record that the iterator will populate.
	return make_shared<MyDB_RecordIterator_Page>(*this, this->myPage, iterateIntoMe);
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) {
	// Get the raw pointer to the page's bytes
    void* rawBytes = this->myPage->getBytes();

    // If this happens, it means clear() was called inappropriately.
    if (rawBytes == nullptr) {
        throw runtime_error("Error: clear() called on a page with no buffer.");
    }

	// Cast rawBytes to a PageHeader.
    PageHeader* header = (PageHeader*)rawBytes;

	// Set page type to toMe.
	header->pageType = toMe;

	// Mark the page as dirty
	this->myPage->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// Get the raw pointer to the page's bytes
    void* rawBytes = this->myPage->getBytes();

    // If this happens, it means clear() was called inappropriately.
    if (rawBytes == nullptr) {
        throw runtime_error("Error: clear() called on a page with no buffer.");
    }

	// Cast rawBytes to a PageHeader.
    PageHeader* header = (PageHeader*)rawBytes;
	
	size_t pageSize = this->myPage->getParent().getPageSize();

	// Get the record size (amount of data in the record buffer).
	size_t recordSize = appendMe->getBinarySize();

	// Can we write this record into the page?
	if (header->endOfData + recordSize  + sizeof(PageHeader) > pageSize ) {
		return false; // not enough sapce
	}

	// Position in page where to write the new record.
	char* pageBytes = (char*)rawBytes;
    char* newRecPos = pageBytes + sizeof(PageHeader) + header->endOfData;

	// Use the record's method to write its binary data.
	appendMe->toBinary(newRecPos);

	// Update offset and numRecordsin header
	header->endOfData += recordSize;
	header->numRecords++;

	// Mark the page as dirty
	this->myPage->wroteBytes();


	return true;
}

#endif
