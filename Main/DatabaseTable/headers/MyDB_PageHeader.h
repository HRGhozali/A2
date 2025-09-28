#ifndef PAGE_HEADER_H
#define PAGE_HEADER_H

#include <memory>
#include <vector>
#include <MyDB_Table.h>

// Avoids including multiple times
#ifndef MYDB_PAGETYPE_WRAPPER
#define MYDB_PAGETYPE_WRAPPER
#include "MyDB_PageType.h" // For the enum
#endif

using namespace std;

struct PageHeader {
    size_t endOfData; 		// Offset: number of used data bytes in the page.
    size_t numRecords;      // Number of records currently stored on the page/
    MyDB_PageType pageType; // Stores the type of page (e.g., RegularPage, DirectoryPage)/
};

#endif