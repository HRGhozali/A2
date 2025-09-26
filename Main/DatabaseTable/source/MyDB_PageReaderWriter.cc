
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"

MyDB_PageReaderWriter::MyDB_PageReaderWriter(MyDB_PagePtr pageToManage): myPage(pageToManage) {
    // The initializer list handles everything. The body can be empty.
}


void MyDB_PageReaderWriter :: clear () {
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
