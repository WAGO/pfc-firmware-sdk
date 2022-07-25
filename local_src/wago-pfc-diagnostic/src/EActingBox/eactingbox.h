//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     eactingbox.h
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef EACTINGBOX_H_
#define EACTINGBOX_H_

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <diagnostic/diagnostic_API.h>
#include <LedCtl_API.h>


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef struct stLEDstate{
    uint8_t symbol;
    char    string[16];
}tLEDstate;
//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
//11 elements
#define LED_STATE_ELEMENTS 18
extern const tLEDstate states[];

int eventmsg_main(int argc, char **argv);
int getidstate_main(int argc, char **argv);
int getledstate_main(int argc, char **argv);
//int reloadConfig_main(int argc, char **argv);
int logforward_main(int argc, char **argv);
int decodeid_main(int argc, char **argv);
int addcstdiag_main(int argc, char **argv);

#endif /* EACTINGBOX_H_ */
//---- End of source file ------------------------------------------------------
