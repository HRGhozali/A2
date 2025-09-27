
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageHeader.h"
#include "MyDB_RecordIterator_Page.h"

MyDB_PageReaderWriter::MyDB_PageReaderWriter(MyDB_PagePtr pageToManage): myPage(pageToManage) {
    // The initializer list handles everything. The body can be empty.
}

// empties out the contents of this page, so that it has no records in it
// the type of the page is set to MyDB_PageType :: RegularPage
void MyDB_PageReaderWriter :: clear () {
	// Get the page header
	PageHeader* header = (PageHeader*) this->myPage->getBytes(this->myPage);

	// Set endOfData and numRecords to zero. Indicates page is empty.
	header->endOfData = 0;
	header->numRecords = 0;

	// Sets the correct page type
	header->pageType = MyDB_PageType::RegularPage;

}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	// Get the page header
	PageHeader* header = (PageHeader*) this->myPage->getBytes(this->myPage);
	return header->pageType;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	// Create a new page iterator, injecting a reference to this MyDB_PageReaderWriter object
	// and the record that the iterator will populate.
	return make_shared<MyDB_RecordIterator_Page>(*this, iterateIntoMe);
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) {
	// Get page header
	PageHeader* header = (PageHeader*) this->myPage->getBytes(this->myPage);
	// Set page type to toMe.
	header->pageType = toMe;
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) {
	// Get the page header
	PageHeader* header = (PageHeader*) this->myPage->getBytes(this->myPage);
	
	size_t pageSize = this->myPage->getParent().getPageSize();

	// Get the record size (amount of data in the record buffer).
	size_t recordSize = appendMe->getBinarySize();

	// Can we write this record into the page?
	if (header->endOfData + recordSize  + sizeof(PageHeader) > pageSize ) {
		return false; // not enough sapce
	}

	// Position in page where to write the new record.
	char* pageBytes = (char*)myPage->getBytes(this->myPage);
    char* newRecPos = pageBytes + sizeof(PageHeader) + header->endOfData;

	// Use the record's method to write its binary data.
	appendMe->toBinary(newRecPos);

	// Update offset and numRecordsin header
	header->endOfData += recordSize;
	header->numRecords++;


	return true;
}

#endif
