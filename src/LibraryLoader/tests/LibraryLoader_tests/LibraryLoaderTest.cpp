//  Copyright (c) 2012-2022 WAGO GmbH & Co. KG
//
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

extern "C"
{
#include "LibraryLoader.h"
#include <glib.h>
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(LibraryLoader)
{
	LibraryLoader libraryLoader;

	void setup()
	{
		libraryLoader = LibraryLoader_Create("/usr/lib/");
	}

	void teardown()
	{
		LibraryLoader_Destroy(libraryLoader);
	}
};

TEST(LibraryLoader, Create)
{
	CHECK(libraryLoader != NULL);
}

TEST(LibraryLoader, ScanDirectory)
{
   int32_t res;
   res = LibraryLoader_ScanDirectory(libraryLoader);
   CHECK(res == LL_SUCCESS);
}

TEST(LibraryLoader, GetLibraryList_CheckListIsNullWithoutPreviousCallToScanDirectory)
{
   int32_t res;
   GSList *list;
   res = LibraryLoader_GetLibraryList(libraryLoader, &list);

   CHECK_EQUAL(LL_FAILURE, res);
   CHECK(NULL == list)
}

// AGa: following unit test fails on Ubuntu 16.04. because it relies on
// /usr/lib containing at least one shared library. On newer Linux systems all
// libraries are in /usr/lib/[Architecture].  Depending on external conditions
// is a bad idea for a unit test anyway.  Besides, the unit test has an
// expectation that after a call to ScanDirectory the list object is not NULL
// even if the list is empty whereas NULL is a valid representation for an
// empty GList object because GLists are nothing but pointers to its first
// leaf.
//
// TODO: create local directories for test input (empty and non-empty) and
// implement separate test cases for them.  Rename following test case to sth.
// like "check list is non null for a non-empty directory."
IGNORE_TEST(LibraryLoader, GetLibraryList_CheckListIsNonNullWithPreviousCallToScanDirectory)
{
   int32_t res;
   GSList *list;

   res = LibraryLoader_ScanDirectory(libraryLoader);
   CHECK_EQUAL(LL_SUCCESS, res);

   res = LibraryLoader_GetLibraryList(libraryLoader, &list);

   CHECK_EQUAL(LL_SUCCESS, res);
   CHECK(NULL !=  list);
}




