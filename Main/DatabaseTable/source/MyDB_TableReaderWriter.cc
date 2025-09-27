
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
	this->myTable = forMe;
	this->myBuffer = myBuffer;
	this->pageSize = myBuffer->getPageSize();
}

// access the i^th page in this file
MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	int lastPageIndex = this->myTable->lastPage()

	//  if i > lastPageIndex, create an empty page up to and including the requested page.
	//  Each of those pages will have no records.
	if (i > lastPageIndex) {
		for (int p = lastPageIndex + 1; p <= i; p++) {
			
			MyDB_PageHandle intermediatePageHandle = this->myBuffer->getPage(this->myTable, p);
			MyDB_PageReaderWriter(intermediatePageHandle->getPage()).clear();
		}
		// Update the table's metadata to reflect the new last page.
		this->myTable->setLastPage(i);
	}

	// Get a handle to the request page "i"
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
	int lastPageIndex = this->myTable->lastPage(); // This could return -1 (empty table)
	if (lastPageIndex < 0) {
        lastPageIndex = 0;
    }

	// Get the last page in the table
	MyDB_PageReaderWriter lastPage = (*this)[lastPageIndex];

	// Append to the last page in the table
	// If that fails, create a new page and append it to there instead
    if (!lastPage.append(appendMe)) {
        MyDB_PageReaderWriter newPage = (*this)[lastPageIndex + 1];
        newPage.clear();
        newPage.append(appendMe);
		this->myTable->setLastPage(this->lastPage);
    }
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
	// Open file
	int file = open(fromMe.c_str(), O_RDONLY);
	if (file < 0) {
		cerr << "Error: cannot open file " << fromMe << " for reading." << endl;
		exit (1);
	}
	
	// Create a buffer to read info into and variables to track the current record + the # of bytes read
	char* buffer = new char[this->pageSize];
	string currLine = "";
	size_t bytesRead;

	// Runs until no more bytes can be read from the file
	while ((bytesRead = read(file,buffer,this->pageSize)) > 0) {  // Reads 1 page at a time
		for (size_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\n') {  // \n denotes separation between records
				// Process current line as a record
				MyDB_RecordPtr newRecord = this->getEmptyRecord();
				newRecord->fromText(currLine);
				this->append(newRecord);
				currLine = "";
			} else {
				currLine += buffer[i];
			}
		}
		// Afterwards, clears buffer to read the next page
		char* temp = new char[this->pageSize];
		buffer = temp;
	}
	delete[] buffer;

	// If there's anything left in currLine, processes it as one final record
	if (currLine != "") {
		MyDB_RecordPtr newRecord = this->getEmptyRecord();
		newRecord->fromText(currLine);
		this->append(newRecord);
	}

	// Close the file
	close(file);
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	// Create iterator that will scan the whole table.
	return make_shared<MyDB_RecordIterator_Table>(*this, iterateIntoMe);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	// Open the file to write to it
	int file = open(toMe.c_str(), O_RDWR | O_CREAT | O_FSYNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (file < 0) {
		cerr << "Error: cannot open file " << toMe << " for writing." << endl;
		exit (1);
	}
	
	// Runs for every page in the table
	for (size_t i = 0; i < this->myTable->lastPage() + 1; i++) {
		// Access the current page w/ a new reader/writer
		MyDB_PageReaderWriter currPage = (*this)[i];

		// Create a record pointer to store records + an iterator to iterate over them
		MyDB_RecordPtr tempRecord = this->getEmptyRecord();
		MyDB_RecordIteratorPtr pageIter = currPage.getIterator(tempRecord);

		// Iterate through all records in the page + write them to the file
		while (pageIter->hasNext()) {
			pageIter->getNext();
			string recordString = to_string(*tempRecord) + "\n";
			write(file, recordString.c_str(), recordString.length());
		}
	}

	close(file);  // Closes the file
}

size_t MyDB_TableReaderWriter :: getNumPages () {
	return this->myTable->lastPage() + 1;
}

#endif