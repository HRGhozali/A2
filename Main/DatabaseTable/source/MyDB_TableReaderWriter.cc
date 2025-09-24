
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) {
	this->myTable = forMe;  // unknown why this does the error...
	this->myBuffer = myBuffer;
	this->pageSize = myBuffer->getPageSize();
	this->numPages = myTable->lastPage() + 1;
	this->lastPage = myTable->lastPage();
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: operator [] (size_t i) {
	MyDB_PageReaderWriter temp;
	return temp;	
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return nullptr;
}

MyDB_PageReaderWriter MyDB_TableReaderWriter :: last () {
	MyDB_PageReaderWriter temp;
	return temp;	
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) {
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) {
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) {
	return nullptr;
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) {
	int file = open(toMe.c_str(), O_RDWR | O_CREAT | O_FSYNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (file < 0) {
		cerr << "Error: cannot open file " << toMe << " for writing." << endl;
		exit (1);
	}
	write(file, ""); // TODO: Write the table contents to the file
	close(file);  // Closes the fie
}

#endif

