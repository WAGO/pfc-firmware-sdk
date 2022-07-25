//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     led_info_json.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_EACTINGBOX_LED_INFO_JSON_H_
#define SRC_EACTINGBOX_LED_INFO_JSON_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <glib.h>
#include <stdint.h>
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
typedef struct {
    char     * name; //name of the LED
    uint32_t   startid; //startid of the LED
    GList    * alias; //list of strings for aliases
}tLEDInfo;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

GList * getInfoLedListFromFile(const char*);
void cleanUpInfoLedList(GList *);
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


#endif /* SRC_EACTINGBOX_LED_INFO_JSON_H_ */
//---- End of source file ------------------------------------------------------

