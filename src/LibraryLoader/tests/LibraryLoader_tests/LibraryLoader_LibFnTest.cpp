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

#include "glib.h"
#include "CppUTestExt/MockSupport_c.h"

#include "PosixMocks.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTest/TestHarness_c.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{

   #if 0
   struct dirent
     {
   #ifndef __USE_FILE_OFFSET64
       __ino_t d_ino;
       __off_t d_off;
   #else
       __ino64_t d_ino;
       __off64_t d_off;
   #endif
       unsigned short int d_reclen;
       unsigned char d_type;
       char d_name[256];      /* We must not include limits.h! */
     };
   #endif

   static struct dirent LibFakeData =
   {
         /*.d_ino =*/ 0,
         /*.d_off =*/ 0,
         /*.d_reclen =*/ 0,
         /*.d_type =*/ DT_REG,
         /*.d_name = */"testlib.so",
   };

   static struct dirent* pLibFakeData = &LibFakeData;

   static const char * DEFAULT_FUNC_NAME = "testlib_open";
   static const char * DEFAULT_DIR = "/usr/lib";
   static void * DEFAULT_LIB_HANDLE = (void*) 0xDeadBeef;
   fnDlSymbol DEFAULT_FUNC = (fnDlSymbol) 0xAffeAffe;

}

TEST_GROUP(LibraryLoader_LibFunctions)
{
	LibraryLoader libraryLoader;
	GSList *libraryList;


	void setup()
	{
	   setupMocks();
	   setupMockData();

	   expectScandirCall();

		libraryList = NULL;
		libraryLoader = LibraryLoader_Create(DEFAULT_DIR);

		int32_t res;
		res = LibraryLoader_ScanDirectory(libraryLoader);
		CHECK_EQUAL(LL_SUCCESS, res);

		res = LibraryLoader_GetLibraryList(libraryLoader, &libraryList);
		CHECK_EQUAL(LL_SUCCESS, res);
		CHECK( libraryList != NULL);
	}

	void setupMocks()
	{
	   UT_PTR_SET(PosixHelper_ScanDir, PosixMock_ScanDir);
	   UT_PTR_SET(PosixHelper_DlOpen, PosixMock_DlOpen);
	   UT_PTR_SET(PosixHelper_DlClose, PosixMock_DlClose);
	   UT_PTR_SET(PosixHelper_DlSym, PosixMock_DlSym);

	   UT_PTR_SET(PosixHelper_DestroyNamelist, PosixMock_DestroyNamelist);
	   UT_PTR_SET(PosixHelper_DestroyNamelistEntry, PosixMock_DestroyNamelistEntry);
	}

	void setupMockData()
	{
	   PosixMock_SetupScandir(mock_c(), &pLibFakeData, 1);
	   PosixMock_SetupDlOpen(mock_c(), DEFAULT_LIB_HANDLE);
      PosixMock_SetupDlClose(mock_c(), 0);
      PosixMocK_SetupDlSym(mock_c(), DEFAULT_FUNC);
	}


	void expectDlOpenCall()
	{
	   mock_c()->expectOneCall("dlopen")
               ->withStringParameters("file", "/usr/lib/testlib.so")
               ->withIntParameters("mode", RTLD_NOW)
               ->andReturnPointerValue(DEFAULT_LIB_HANDLE);
	}

	void expectDlCloseCall()
	{
	   mock_c()->expectOneCall("dlclose")
               ->withPointerParameters("handle", DEFAULT_LIB_HANDLE)
               ->andReturnIntValue(0);
	}

	void expectDlSymCall()
   {
      mock_c()->expectOneCall("dlsym")
               ->withPointerParameters("handle", DEFAULT_LIB_HANDLE)
               ->withStringParameters("name", DEFAULT_FUNC_NAME)
               ->andReturnPointerValue( (void*) DEFAULT_FUNC);
   }

	void expectScandirCall()
	{
	   mock_c()->expectOneCall("scandir")
               ->withStringParameters("dir", DEFAULT_DIR)
               ->withPointerParameters("selector", NULL)
               ->withPointerParameters("cmp", (void*)alphasort)
               ->andReturnIntValue(1);
	}


	void teardown()
	{

		LibraryLoader_Destroy(libraryLoader);

		mock_c()->checkExpectations();
		mock_c()->clear();

	   teardownMocks();
	}

	void teardownMocks()
	{
	   PosixMock_SetupScandir(NULL, NULL, 0);

	   PosixMock_SetupDlClose(NULL, 0);
	   PosixMock_SetupDlOpen(NULL, NULL);
	   PosixMocK_SetupDlSym(NULL, NULL);

	}

	LibraryInfo GetFirstLibrary()
	{
	   return LibraryLoader_GetLibrayInfo(libraryList);
	}

	LibraryInfo GetAndOpenFirstLibrary()
	{
	   expectDlOpenCall();
	   expectDlCloseCall();

	   LibraryInfo lib = GetFirstLibrary();
      CHECK(lib != NULL);

      int32_t res;
      res = LibraryLoader_LibraryOpen( lib);
      CHECK_EQUAL(LL_SUCCESS, res);

      return lib;
	}
};



TEST(LibraryLoader_LibFunctions, OpenCloseLibrary)
{
   expectDlOpenCall();
   expectDlCloseCall();

   LibraryInfo lib = GetFirstLibrary();
   CHECK(lib != NULL);

   int32_t res;
   res = LibraryLoader_LibraryOpen( lib);
   CHECK_EQUAL(LL_SUCCESS, res);

   bool isOpen;
   isOpen = LibraryLoader_IsLibraryOpen( lib);
   CHECK_EQUAL(true, isOpen);

   res = LibraryLoader_LibraryClose(lib);
   CHECK_EQUAL(LL_SUCCESS, res);

   isOpen = LibraryLoader_IsLibraryOpen(lib);
   CHECK_EQUAL(false, isOpen);
}


TEST(LibraryLoader_LibFunctions, GetSymbolFromLibrary)
{
   expectDlSymCall();

   LibraryInfo lib = GetAndOpenFirstLibrary();
   CHECK(lib != NULL);

   fnDlSymbol func;
   int32_t res;
   res = LibraryLoader_LoadSymbol(lib, DEFAULT_FUNC_NAME, &func);

   CHECK_EQUAL(LL_SUCCESS, res);
   CHECK_EQUAL(DEFAULT_FUNC, func);
}


TEST(LibraryLoader_LibFunctions, GetLibraryFullName)
{
   LibraryInfo lib = GetFirstLibrary();
   CHECK(lib != NULL);

   int32_t res;
   const char* name;
   res = LibraryLoader_GetLibraryFullName(lib, &name);

   CHECK_EQUAL(LL_SUCCESS, res);
   CHECK_EQUAL_C_STRING("/usr/lib/testlib.so", name);
}

TEST(LibraryLoader_LibFunctions, GetLibraryShortName)
{
   LibraryInfo lib = GetFirstLibrary();
   CHECK(lib != NULL);

   int32_t res;
   const char* name;
   res = LibraryLoader_GetLibraryShortName(lib, &name);

   CHECK_EQUAL(LL_SUCCESS, res);
   CHECK_EQUAL_C_STRING("testlib.so", name);
}

TEST(LibraryLoader_LibFunctions, GetLibraryBaseName)
{
   LibraryInfo lib = GetFirstLibrary();
   CHECK(lib != NULL);

   int32_t res;
   const char* name;
   res = LibraryLoader_GetLibraryBaseName(lib, &name);

   CHECK_EQUAL(LL_SUCCESS, res);
   CHECK_EQUAL_C_STRING("testlib", name);
}


