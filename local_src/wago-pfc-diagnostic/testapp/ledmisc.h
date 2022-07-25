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
///  \file     ledmisc.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LEDMISC_H_
#define LEDMISC_H_

#include <ledserver_API.h>
extern int max_id;

typedef struct
{
    tLedStaticColor color;
    char name[64];
}tLedColorNames;
typedef int tColorIter;

tColorIter GetColorIter(void);
int NextColorIter(tColorIter * iter);
const char * GetColorName(tColorIter iter);
const char * GetColorNameByValue(tLedStaticColor val);
tLedStaticColor GetColorVal(tColorIter iter);
int GenerateValidOnlyXmlFile(void);
int GenerateInvalidOnlyXmlFile(void);
int GenerateXmlFile(void);
int GenerateSpecificXmlFile(int valid, tLedStateClass state);
int CheckLedServer(void);
int sendAllIds(int times);

#endif /* LEDMISC_H_ */
//---- End of source file ------------------------------------------------------
