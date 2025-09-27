
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_RecordIterator_Table.h"
#include "MyDB_Page.h"


using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;  // unknown why this does the error...
	this->myBuffer = myBuffer;
	this->pageSize = myBuffer->getPageSize();
	this->numPages = myTable->lastPage() + 1;
	this->lastPage = myTable->lastPage();
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	if (i >= this->numPages) {
		cerr << "Error: trying to access page " << i << " in table " << this->myTable->getName() << " which has only " << this->numPages << " pages." << endl;
		exit (1);
	}
	MyDB_PageHandle requestedPage = this->myBuffer->getPage(this->myTable, i);

	MyDB_PageReaderWriter accessPage = MyDB_PageReaderWriter(requestedPage->getPage()); 
	return accessPage;	
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return make_shared<MyDB_Record>(myTable->getSchema());
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	return (*this)[this->myTable->lastPage()];
}

void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) { 
	int lastPageIndex = this->lastPage; // This could return -1 (empty table)
	if (lastPageIndex < 0) {
        lastPageIndex = 0;
    }

	MyDB_PageReaderWriter lastPage = (*this)[lastPageIndex];

	// If the append fails, get a new page and append there
    if (!lastPage.append(appendMe)) {
        MyDB_PageReaderWriter newPage = (*this)[lastPageIndex + 1];
        newPage.clear();
        newPage.append(appendMe);
    }

}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	int file = open(fromMe.c_str(), O_RDONLY);
	if (file < 0) {
		cerr << "Error: cannot open file " << fromMe << " for reading." << endl;
		exit (1);
	}
	
	// TODO: read stuff

	close(file);
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	// Create iterator that will scan the whole table.
	return make_shared<MyDB_RecordIterator_Table>(*this, iterateIntoMe);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	int file = open(toMe.c_str(), O_RDWR | O_CREAT | O_FSYNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (file < 0) {
		cerr << "Error: cannot open file " << toMe << " for writing." << endl;
		exit (1);
	}
	
	//write(file, ""); // TODO: Write the table contents to the file
	close(file);  // Closes the fie
}

size_t MyDB_TableReaderWriter :: getNumPages () {
	return this->numPages;
}

#endif

