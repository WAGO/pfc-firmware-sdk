//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     ledmisc.h
///
///  \version  $Revision: 12559 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LEDMISC_H_
#define LEDMISC_H_

#include <ledserver_API.h>

typedef struct
{
    tLedReturnCode code;
    char name[64];
}tLedReturnNames;

typedef int tColorIter;

typedef struct
{
    tLedStaticColor color;
    char name[64];
}tLedColorNames;

typedef struct
{
    tLedStateClass class;
    char name[64];
}tLedStateNames;


const char * GetErrorString(tLedReturnCode code);
tColorIter GetColorIter(void);
int NextColorIter(tColorIter * iter);
const char * GetColorName(tColorIter iter);
const char * GetColorNameByValue(tLedStaticColor val);
tLedStaticColor GetColorVal(tColorIter iter);
const char * GetStateString(tLedStateClass code);

#endif /* LEDMISC_H_ */
//---- End of source file ------------------------------------------------------
