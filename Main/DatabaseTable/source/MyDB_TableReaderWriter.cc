
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_RecordIterator_Table.h"
#include "MyDB_Page.h"


using namespace std;

// Constructor uses initializer list 
MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) :
	myTable(forMe),
	myBuffer(myBuffer)
{
	this->pageSize = myBuffer->getPageSize();
}

// access the i^th page in this file
MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	int lastPageIndex = this->myTable->lastPage();

	//  if i > lastPageIndex, create an empty page up to and including the requested page.
	//  Each of those pages will have no records.
	if ((int)i > lastPageIndex) {
		//cout << "i > lastPageIndex; making new pages" << endl;
		for (size_t p = lastPageIndex + 1; p <= i; p++) {
			//cout << "Making page " << p << endl;
			MyDB_PageHandle intermediatePageHandle = this->myBuffer->getPage(this->myTable, p);
			MyDB_PageReaderWriter newPage(intermediatePageHandle->getPage());
			newPage.clear();
		}
		// Update the table's metadata to reflect the new last page.
		//cout << "New last page: " << i << endl;
		this->myTable->setLastPage(i);
	}

	// Get a handle to the requested page "i"
	MyDB_PageHandle requestedPage = this->myBuffer->getPage(this->myTable, i);

	return  MyDB_PageReaderWriter(requestedPage->getPage());
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
		this->myTable->setLastPage(lastPageIndex + 1);
    }
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {

	// First, clear the table by resetting its last page index
    myTable->setLastPage(-1);


	// Open file
	ifstream file(fromMe);
    if (!file.is_open()) {
        cerr << "Error: could not open text file " << fromMe << endl;
        return;
    }

	// Get the first page to start writing to
    int currentPageIndex = 0;
    MyDB_PageReaderWriter currentPage = (*this)[currentPageIndex];
    currentPage.clear(); // Ensure it's empty
	
	// Create an empty record and keep the current line to read.
	string currLine;
	MyDB_RecordPtr newRec = this->getEmptyRecord();

	// Runs until no more lines can be read from the file
	while (getline(file, currLine)) {  // Reads 1 line at a time
		// Use fromString to parse the text line
		newRec->fromString(currLine);

		// Try to append the record. If the page is full get the next page and try again
		while (!currentPage.append(newRec)) {
			currentPageIndex++;
			currentPage = (*this)[currentPageIndex]; 
			// Clear page to ensure its empty before trying to append to it.
			currentPage.clear();
		}
	}
	file.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	// Create iterator that will scan the whole table.
	cout << "TABLE READ/WRITE DEBUG: Getting iterator" << endl;
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
			size_t tempSize = tempRecord->getBinarySize();
			char* temp = new char[tempSize];
			tempRecord->toBinary(temp);
			write(file, temp, tempSize);
			delete[] temp;
		}
	}

	close(file);  // Closes the file
}

size_t MyDB_TableReaderWriter :: getNumPages () {
    return (this->myTable->lastPage() < 0) ? 0 : (size_t)this->myTable->lastPage() + 1;
}

#endif