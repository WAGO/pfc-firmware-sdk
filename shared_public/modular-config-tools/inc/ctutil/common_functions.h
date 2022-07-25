//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project modular-config-tools (PTXdist package modular-config-tools).
//
// Copyright (c) 2020-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     common_functions.h
///
///  \brief    Commonly used function prototypes and function pointer types.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef INC_CTUTIL_COMMON_FUNCTIONS_H_
#define INC_CTUTIL_COMMON_FUNCTIONS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/assertion.h"
#include "ctutil/common_main.h"
#include "ctutil/error_codes.h"
#include <getopt.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_Mount
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_Mount_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                         char const * const szSpecialFile,
                                         char const * const szDir,
                                         char const * const szFileSystemType,
                                         unsigned long int const flags,
                                         void const * const pData);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_Umount
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_Umount_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                          char const * const szSpecialFile,
                                          int const flags);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_GetFreeBytes
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_GetFreeBytes_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                char const * const szFolderPath,
                                                uint64_t * const freeBytes);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_GetAvailableBytes
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_GetAvailableBytes_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                     char const * const szFolderPath,
                                                     uint64_t * const availableBytes);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_GetMountPointForDevice
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_GetMountPointForDevice_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                          char const * const szDeviceFilePath,
                                                          char * * const pszMountPoint,
                                                          size_t const mountPointBufferSize);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_IsFileAvailable
//------------------------------------------------------------------------------
typedef bool (*ctutil_IsFileAvailable_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                           char const * const szFilePath,
                                           char const * const szFileTypes);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_IsFolderAccessible
//------------------------------------------------------------------------------
typedef bool (*ctutil_IsFolderAccessible_ptr)(char const * const szPath);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_CreateFolder
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_CreateFolder_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                char const * const szPath,
                                                bool createParentsAlso);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_ClearFolder
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_ClearFolder_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                               char const * const szPath);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_RemoveFolder
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_RemoveFolder_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                char const * const szPath);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_IsFileTypeMatch
//------------------------------------------------------------------------------
typedef bool (*ctutil_IsFileTypeMatch_ptr)(struct stat * pstStats,
                                           char const fileType);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_GetFileSystemStatistics
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_GetFileSystemStatistics_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                           char const * const szPath,
                                                           struct statvfs * const pstFileSystemStats);

//------------------------------------------------------------------------------
/// Type definition for a function to:
/// \copydoc ctutil_CheckForMountPoint
//------------------------------------------------------------------------------
typedef statusCode_t (*ctutil_CheckForMountPoint_ptr)(ctutil_OptionsCommon_t const * const pstCommonOptions,
                                                      char const * const szPath);


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  //------------------------------------------------------------------------------
  /// Default implementation to print short info text about program.
  ///
  /// \param szCallString
  ///   Call string from program call, normally this is "argv[0]".
  /// \param szInfoText
  ///   Info text string with short program description.
  ///
  /// \see ctparts_commonMainContext_t
  //------------------------------------------------------------------------------
  void ctutil_PrintInfoTextDefault(char const * const szCallString,
                                   char const * const szInfoText);

  //------------------------------------------------------------------------------
  /// Default implementation to print program help.
  ///
  /// \param szCallString
  ///   Call string from program call, normally this is "argv[0]".
  /// \param szHelpText
  ///   Main help text string with usage description.
  /// \param szAdditionalText
  ///   Additional help text string with additional information
  ///   (e.g. program target and description).
  /// \param szExampleText
  ///   Example text string with some useful example program calls.
  /// \param arstCommandlineOptions
  ///   Program option table to generate option description.
  ///   This table has to be NULL-terminated.
  /// \param arszCommandlineOptionTexts
  ///   Program option text table with short option description corresponding to
  ///   program option table.
  ///   This table has to be NULL-terminated.
  ///
  /// \see ctparts_commonMainContext_t
  //------------------------------------------------------------------------------
  void ctutil_PrintHelpDefault(char const * const szCallString,
                               char const * const szHelpText,
                               char const * const szAdditionalText,
                               char const * const szExampleText,
                               struct option const * const arstCommandlineOptions,
                               char const * const * const arszCommandlineOptionTexts);

  //------------------------------------------------------------------------------
  /// Helper function for standard free to avoid warnings when free const pointers.
  ///
  /// \param pMemoryToFree
  ///   Pointer to memory to free.
  //------------------------------------------------------------------------------
  void ctutil_FreeConst(void const * const pMemoryToFree);

  //------------------------------------------------------------------------------
  /// Helper function to copy program string parameter to target pointer.
  ///
  /// \param pszStringParam
  ///   Pointer to target string pointer. Memory is allocated by standard malloc.
  ///   If char pointer is not NULL the pointer is freed before new allocation.
  /// \param szOptionArg
  ///   String option argument to copy.
  ///
  /// \return
  ///   \link CTUTIL_SUCCESS \endlink in case string copy was successful.
  ///
  /// \see CTUTIL_SUCCESS
  //------------------------------------------------------------------------------
  statusCode_t ctutil_CopyStringParam(char const * * const pszStringParam,
                                      char const * const szOptionArg);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#endif /* INC_CTUTIL_COMMON_FUNCTIONS_H_ */
//---- End of source file ------------------------------------------------------

