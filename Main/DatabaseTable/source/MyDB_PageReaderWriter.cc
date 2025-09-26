
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_PageHeader.h"

MyDB_PageReaderWriter::MyDB_PageReaderWriter(MyDB_PagePtr pageToManage): myPage(pageToManage) {
    // The initializer list handles everything. The body can be empty.
}


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
	return MyDB_PageType :: RegularPage;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return nullptr;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) {
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) {
	return true;
}

#endif
