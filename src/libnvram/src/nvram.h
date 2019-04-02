//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of PTXdist package libnvram.
//
// Copyright (c) 2008-2018 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     nvram.h
///
///  \brief    NVRAM interface for WAGO PFC200, this interface is not thread safe
///
///  \author   HJH : WAGO Kontakttechnik GmbH & Co. KG
///  \author   HFS : WAGO Kontakttechnik GmbH & Co. KG
///  \author   LHe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#ifndef _NVRAM_H
#define _NVRAM_H


//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure,enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


  //------------------------------------------------------------------------------
  /// Initialize user io space for nvram so it can be used with "nvram_get_mapping".
  ///
  /// \return
  ///   Return 0 on success, -EIO otherwise.
  ///
  /// \see nvram_close
  //------------------------------------------------------------------------------
  extern int8_t nvram_init(void);

  //------------------------------------------------------------------------------
  /// Same as nvram_init() but map nvram at given address "map_addr".
  ///
  /// \return
  ///   Return 0 on success, -EIO otherwise.
  ///
  /// \see nvram_close
  //------------------------------------------------------------------------------
  extern int8_t nvram_init_with_addr(void* map_addr);

  //------------------------------------------------------------------------------
  /// This function must be called after "nvram_init" to free nvram.
  ///
  /// \see nvram_init
  //------------------------------------------------------------------------------
  extern void nvram_close(void);

  //------------------------------------------------------------------------------
  /// Get the base address for nvram memory.
  ///
  /// \return
  ///   Return a void pointer with the base address of nvram.
  //------------------------------------------------------------------------------
  extern void * nvram_get_mapping(void);

  //------------------------------------------------------------------------------
  /// Get size of nvram in bytes.
  ///
  /// \return
  ///   Return the size of nvram as unsigned long.
  //------------------------------------------------------------------------------
  extern unsigned long nvram_get_size(void);

  //------------------------------------------------------------------------------
  /// Get last error message.
  ///
  /// \return
  ///   Return the last error message, max. 128 bytes. The message is null-terminated.
  //------------------------------------------------------------------------------
  extern const char *nvram_get_errmsg(void);

  //------------------------------------------------------------------------------
  /// Overwrite complete nvram with 0.
  //------------------------------------------------------------------------------
  extern void nvram_erase(void);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif
