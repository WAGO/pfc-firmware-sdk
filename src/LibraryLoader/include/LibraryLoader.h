#ifndef D_LibraryLoader_H
#define D_LibraryLoader_H

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
/// \brief The LibraryLoader provides the ability to load libraries from a certain
///        directory.
///
/// \author ${user} $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <glib.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

enum
{
   LL_SUCCESS = 0, LL_FAILURE = -1,
};

/**
 * Maximum values used by the library loader.
 */
enum
{
   LL_MAX_BASENAME_LENGTH = 253, //!< LL_MAX_BASENAME_LENGTH Max length of the library filename w/o ending
   LL_MAX_FILENAME_LENGTH = LL_MAX_BASENAME_LENGTH + 3, //!< LL_MAX_FILENAME_LENGTH Max length of the library filename
};

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stLibraryLoader * LibraryLoader;
typedef struct stLibraryInfo tLibraryInfo;
typedef tLibraryInfo * LibraryInfo;


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

/**
 * Scans the directory specified in LibraryLoader_Create for *.so files.
 * @param self Instance of the LibraryLoader
 * @return
 */
int32_t LibraryLoader_ScanDirectory(LibraryLoader self);

/**
 * Gets the list of libraries found during LibraryLoader_ScanDirectory.
 * @note This function exposes the internal GSList structure to the user,
 *   so don't do anything funny with it.
 * @param self Instance of the LibraryLoader
 * @param list The GSList of the found libraries. The list->data field is of type tLibraryInfo*.
 * @return Returns LL_SUCCESS when the internal list contains any elements. LL_FAILURE otherwise.
 */
int32_t LibraryLoader_GetLibraryList(LibraryLoader self,
                                     GSList** list);


/**
 * Opens the supplied library.
 * @param toOpen The lib to open
 * @return LL_SUCCES on succes, LL_FAILURE otherwise.
 */
int32_t LibraryLoader_LibraryOpen(LibraryInfo toOpen);


/**
 * Closes the supplied library
 * @param toClose The lib to close.
 * @return LL_SUCCESS on success, LL_FAILURE otherwise.
 */
int32_t LibraryLoader_LibraryClose(LibraryInfo toClose);


/**
 * Queries if the supplied lib has been opened.
 * @param lib The lib to get the 'open-state' from.
 * @return true when the lib has bee opened. false otherwise.
 */
bool LibraryLoader_IsLibraryOpen(const tLibraryInfo *lib);


/**
 * Gets the full path of the lib.
 * /usr/lib/foo.so -> name = "/usr/lib/foo.so".
 * @param lib The lib to get the path from.
 * @param name The resulting name.
 * @return LL_SUCCESS if the path could be determined. LL_FAILURE otherwise.
 */
int32_t LibraryLoader_GetLibraryFullName(const tLibraryInfo * lib,
                                         const char** name);


/**
 * Gets the filename of the lib.
 * /usr/lib/foo.so -> name = "foo.so".
 * @param lib The lib to get the filename from.
 * @param name The resulting name.
 * @return LL_SUCCESS if the path could be determined. LL_FAILURE otherwise.
 */
int32_t LibraryLoader_GetLibraryShortName(const tLibraryInfo * lib,
                                          const char** name);

/**
 * Gets the base name of the lib.
 * /usr/lib/foo.so -> name = "foo".
 * @param lib The lib to get the base name from.
 * @param name The resulting name.
 * @return LL_SUCCESS if the path could be determined. LL_FAILURE otherwise.
 */
int32_t LibraryLoader_GetLibraryBaseName(const tLibraryInfo * lib,
                                         const char** name);

/**
 * Loads a given symbol from the lib.
 * @param lib The lib to load the symbol from.
 * @param symbol Name of the symbol.
 * @param func The loaded symbol.
 * @return LL_SUCCESS of the symbol could be loaded. LL_FAILURE otherwise.s
 */
int32_t LibraryLoader_LoadSymbol(const tLibraryInfo * lib,
                                 const char* symbol,
                                 void (**func)(void));


/**
 * Creates a new instance of the library loader.
 * @param baseDirectory The directory to load the libs from.
 * @return Returns the new instance on success. NULL otherwise (out of mem).
 */
LibraryLoader LibraryLoader_Create(const char* baseDirectory);

/**
 * Destroys the supplied LibraryLoader.
 * All opened lib will be closed during destruction.
 * @param self The LibraryLoader instance to destroy.
 */
void LibraryLoader_Destroy(LibraryLoader self);


/**
 * Gets the library from a list entry.
 * @param listElement
 * @return Returns the LibraryInfo on success. Otherwise NULL.
 */
static inline LibraryInfo LibraryLoader_GetLibrayInfo(GSList* listElement)
{
   return (LibraryInfo) (listElement == NULL ? NULL : listElement->data);
}

#endif  // D_LibraryLoader_H
