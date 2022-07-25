//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev$
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include "LibraryLoader.h"

#include <assert.h>
#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "LibraryLoader_Helper.h"
#include "LibraryLoader_Logging.h"
#include "PosixHelper.h"

#include "common_helper.h"
#include "cpputest_mem_leak_quirk.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stLibraryInfo
{
   char* FilePath;
   char* BaseName;
   void * Handle;
   void (*OpenFunc);
} stLibraryInfo;

typedef struct stLibraryLoader
{
   const char* LibraryDirectoryPath;
   GSList* LibraryList;
} stLibraryLoader;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

static LibraryInfo GetLibraryInfoByName(LibraryLoader self,
                                        const char * const libName);

static bool IsRegularFile(int32_t dt_type, const char* filePath);

static void GSListHelper_Free(gpointer data,
                             gpointer nil);

//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------

static void GSListHelper_Free(gpointer data,
                             gpointer nil)
{
   UNUSED_PARAMETER(nil);
   if (data != NULL )
   {
      LibraryInfo lib = (LibraryInfo)data;
      __UNMATCHED_FREE(lib->BaseName);
      __UNMATCHED_FREE(lib->FilePath);
      LibraryLoader_LibraryClose(lib);
      free(data);
   }
}

/**
 *
 * @param dt_type
 * @return
 */
static bool IsRegularFile(int32_t dt_type, const char* filePath)
{
   bool isRegularFile = (dt_type == DT_REG);
   if ( (dt_type == DT_UNKNOWN) || (dt_type == DT_LNK) )
   {
      struct stat fileState;
      if ( 0 == stat(filePath, &fileState))
      {
         isRegularFile = S_ISREG(fileState.st_mode);
      }
      else
      {
         ERROR("stat() failed with %s", strerror(errno));
      }
   }
   return isRegularFile;
}

/**
 * GSList custom find function.
 * @param data
 * @param libName
 * @return
 */
static gint FindLibraryByName(gconstpointer data,
                              gconstpointer libName)
{
   return strcmp(((LibraryInfo) data)->BaseName, libName);
}

/**
 * @brief Returns the device information structure for a device name
 *
 * @param devName Name of the device
 *
 * @return Device information structure
 */
static LibraryInfo GetLibraryInfoByName(LibraryLoader self,
                                        const char * const libName)
{


   GSList *result = g_slist_find_custom(self->LibraryList, libName,
         FindLibraryByName);

   return (result != NULL ) ? result->data : NULL ;
}

/**
 * Creats a new LibraryInfo
 * @param filePath The path of the *.so
 * @param fileBasename The base name of the *.so
 * @return The newly created LibraryInfo on success, NULL otherwise.
 */
static LibraryInfo ConstructNewLibInfo(const char * const filePath,
                                       const char * const fileBasename)
{
   LibraryInfo newLibInfo;

   newLibInfo = safe_calloc(1, sizeof(*newLibInfo));

   newLibInfo->FilePath = strndup(filePath, strlen(filePath) + 1);
   newLibInfo->BaseName = strndup(fileBasename, strlen(fileBasename) + 1);

   const bool allocFailed = (newLibInfo->FilePath == NULL )
         || (newLibInfo->BaseName == NULL );

   if (allocFailed)
   {
      ERROR("%s", "Not enough memory.");
      if (newLibInfo->FilePath != NULL )
         free(newLibInfo->FilePath);

      if (newLibInfo->BaseName != NULL )
         free(newLibInfo->BaseName);

      free(newLibInfo);
      newLibInfo = NULL;
   }

   return newLibInfo;
}

/**
 * @brief  Extracts a device name (base name, i.e. file name w/o the .so extension)
 * from a full file path.
 *
 *
 * @param path       A path to extract the base name from.
 * @param devName    The result buffer to copy the device name.
 * @param devNameLen Length of the devName buffer
 *
 * @return           LL_SUCCESS if no error occured; LL_FAILURE otherwise.
 */__STATIC int32_t LibPathToBasename(const char * const path,
                                    char *devName,
                                    const size_t devNameLen)
{
   assert(path != NULL);
   assert(devName != NULL);

   CHECK_POINTER(path);
   CHECK_POINTER(devName);

   int32_t res = LL_FAILURE;
   const char *fileExtensionPos;
   const size_t extLen = strlen(".so");

   const char *filename = strrchr(path, '/');

   if (NULL == filename)
   {
      /* path starts with file name */
      filename = path;
   }
   else
   {
      ++filename;
   }

   fileExtensionPos = strstr(filename, ".so");
   if (fileExtensionPos != &filename[strlen(filename) - extLen])
   { /* file does not end with .so */
      DBG("File '%s': not the expected .so extension.", filename);
   }
   else if (devNameLen <= (strlen(filename) - extLen))
   {
      ERROR("Filename %s too long.", filename);
   }
   else
   {
      strncpy(devName, filename, strlen(filename) - extLen);
      devName[strlen(filename) - extLen] = '\0';
      res = LL_SUCCESS;
   }

   return res;
}

int32_t LibraryLoader_ScanDirectory(LibraryLoader self)
{
   assert(NULL != self);
   assert(NULL != self->LibraryDirectoryPath);

   CHECK_POINTER(self);

   struct dirent **namelist;

   int32_t n;

   int32_t ret = LL_SUCCESS;

   LibraryInfo libInfo = NULL;

   char fileBasename[LL_MAX_BASENAME_LENGTH + 1];
   size_t nameLen;

   // +2 for terminating 0 and for the additional slash btw. path and file name
   char filePath[strlen(self->LibraryDirectoryPath) + LL_MAX_FILENAME_LENGTH + 2];

   /*lint -e961 suppress "More than two pointer indirection levels used"
    * -> scandir is POSIX so we have to live  with three pointer indirections */
   n = PosixHelper_ScanDir(self->LibraryDirectoryPath, &namelist, NULL, alphasort); /* get name of all files in search path */
   /*lint -restore */

   if (n < 0)
   {
      ERROR("%s", strerror(errno));
      ret = LL_FAILURE;
   }
   else if (n == 0)
   {
      ERROR("%s is empty.", self->LibraryDirectoryPath);
      ret = LL_FAILURE;
   }
   else
   {
      while (n--)
      {
         // sizeof(filePath) > strlen(LibraryDirectoryPath) + 1 => setting 3rd param of strncpy to
         // strlen(source) + 1 makes sure strncpy null-terminates the result string.
         strncpy(filePath, self->LibraryDirectoryPath,
               strlen(self->LibraryDirectoryPath) + 1);

         // append an additional slash when the directory string is missing it
         strcat(filePath, "/");

 //        DBG("Processing %s", namelist[n]->d_name);

         nameLen = strlen(namelist[n]->d_name);

         if (nameLen > LL_MAX_FILENAME_LENGTH)
         {
            ERROR(
                  "%s: device name too long (Maximal device name length is %d characters).",
                  namelist[n]->d_name, LL_MAX_FILENAME_LENGTH);

            PosixHelper_DestroyNamelistEntry(namelist[n]);
            continue;
         }

         strncat(filePath, namelist[n]->d_name, nameLen);
         filePath[sizeof(filePath) - 1] = '\0';

         if (!IsRegularFile(namelist[n]->d_type, filePath))
         {
            // __UNMATCHED_FREE is used because namelist[n] is allocated inside scandir()
            PosixHelper_DestroyNamelistEntry(namelist[n]);
            continue;
         }

         if (LL_FAILURE
               == LibPathToBasename(filePath, fileBasename,
                     sizeof(fileBasename)))
         {
            DBG("Ignoring %s: not an .so file.", filePath);
            PosixHelper_DestroyNamelistEntry(namelist[n]);
            continue;
         }

         if (NULL == (libInfo = GetLibraryInfoByName(self, fileBasename)))
         {  // Device is not yet known => construct new object
            libInfo = ConstructNewLibInfo(filePath, fileBasename);

            self->LibraryList = g_slist_append(self->LibraryList, libInfo);
         }

         PosixHelper_DestroyNamelistEntry(namelist[n]);

      } // end while(n--)

      PosixHelper_DestroyNamelist(namelist);
   } // if (( n = scandir()) >= 0)
   return ret;
}

int32_t LibraryLoader_GetLibraryList(LibraryLoader self, GSList **list)
{
   assert(self);
   assert(list != NULL);

   CHECK_POINTER(list);
   CHECK_POINTER(self);

   *list = self->LibraryList;

   return *list == NULL
         ? LL_FAILURE
         : LL_SUCCESS;
}

//int32_t LibraryLoader_GetFirstLibrary(LibraryLoader self)
//{
//   assert(self != NULL);
//   CHECK_POINTER(self);
//
//   return LL_SUCCESS;
//}

int32_t LibraryLoader_LibraryOpen(LibraryInfo toOpen)
{
   CHECK_POINTER(toOpen);

   void *libHandle = NULL;

   if(NULL != toOpen->FilePath)
   {
      libHandle = PosixHelper_DlOpen(toOpen->FilePath, RTLD_NOW);

      if(NULL == libHandle)
      {
         ERROR("%s", dlerror());
      }
   }

   toOpen->Handle = libHandle;

   return (libHandle == NULL)
         ? LL_FAILURE
         : LL_SUCCESS;
}

int32_t LibraryLoader_LibraryClose(LibraryInfo toClose)
{
   assert(toClose != NULL);
   CHECK_POINTER(toClose);

   int32_t res = LL_SUCCESS;
   if ( LibraryLoader_IsLibraryOpen(toClose))
   {
         if(0 != PosixHelper_DlClose(toClose->Handle)) /* unlikely */
         {
            ERROR("dlclose: %s", dlerror());
            res = LL_FAILURE;
         }
         else
         {
            toClose->Handle = NULL;
         }
   }

   return res;
}

bool LibraryLoader_IsLibraryOpen(const tLibraryInfo *lib)
{
   assert(lib != NULL);
   CHECK_POINTER(lib);
   return lib->Handle != NULL;
}


int32_t LibraryLoader_GetLibraryFullName(const tLibraryInfo * lib,
                                         const char** name)
{
   assert(lib != NULL);
   assert(name != NULL);
   CHECK_POINTER(lib);
   CHECK_POINTER(name);

   *name = lib->FilePath;

   return LL_SUCCESS;
}

int32_t LibraryLoader_GetLibraryShortName(const tLibraryInfo * lib,
                                          const char** name)
{
   assert(lib != NULL);
   assert(name != NULL);
   CHECK_POINTER(lib);
   CHECK_POINTER(name);

   /* short name is the plain filename without the containing directory
    * -> search for '/' from the end. */
   char * filename = strrchr(lib->FilePath, '/');

   *name = filename != NULL
         ? filename + 1 // skip '/'
         : lib->FilePath;

   return LL_SUCCESS;
}

int32_t LibraryLoader_GetLibraryBaseName(const tLibraryInfo * lib,
                                         const char** name)
{
   assert(lib != NULL);
   assert(name != NULL);
   CHECK_POINTER(lib);
   CHECK_POINTER(name);

   *name = lib->BaseName;

   return LL_SUCCESS;
}

int32_t LibraryLoader_LoadSymbol(const tLibraryInfo * lib,
                                 const char* symbol,
                                 fnDlSymbol *func)
{
   assert(lib != NULL);
   assert(symbol != NULL);
   assert(func != NULL);

   CHECK_POINTER(lib);
   CHECK_POINTER(symbol);
   CHECK_POINTER(func);

   union
   {
      void *Pointer;
      fnDlSymbol Function;
   }castHelper = {NULL};


   if ( LibraryLoader_IsLibraryOpen(lib))
   {
      castHelper.Function = PosixHelper_DlSym(lib->Handle, symbol);
   }
   *func = castHelper.Function;

   if ( castHelper.Pointer == NULL)
   {
      ERROR("dlsym: %s", dlerror());
   }

   return (castHelper.Pointer == NULL)
         ? LL_FAILURE
         : LL_SUCCESS;
}

LibraryLoader LibraryLoader_Create(const char* baseDirectory)
{

   LibraryLoader self = calloc(1, sizeof(*self));


   size_t length = strlen(baseDirectory);

   /* remove tailing '/'/ */
   if ( baseDirectory[length-1] == '/')
      length--;

   self->LibraryDirectoryPath = strndup(baseDirectory, length);

   if (self->LibraryDirectoryPath == NULL )
   {
      free(self);
      self = NULL;
   }

   return self;
}

void LibraryLoader_Destroy(LibraryLoader self)
{
   if ( self != NULL)
   {
      __UNMATCHED_FREE((void*)self->LibraryDirectoryPath);      /* TODO Update glib in ptxdist */
      //g_slist_free_full(self->LibraryList, free);
      g_slist_foreach(self->LibraryList, GSListHelper_Free, NULL);
   }
   free(self);
}

