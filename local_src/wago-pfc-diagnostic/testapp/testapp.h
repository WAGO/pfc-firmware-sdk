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
///  \file     testapp.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef TESTAPP_H_
#define TESTAPP_H_
#include <stdio.h>
typedef int(*tTestFunction)(void);

typedef struct
{
    char name[255];
    char help[1024];
    tTestFunction function;
}tTestCase;

extern tTestCase interactiveTests[];
extern size_t numberOfInteractiveTests;
extern tTestCase autoTests[];
extern size_t numberOfAutoTests;

tTestFunction  selectTest(tTestCase * menu, size_t entries);
int RunTests(tTestCase * menu, size_t entries);
int TestExit(void);
int YesNo(void);
void TestLogger(const char * comment);
void SetLogger(int onoff);
void LogTest(int result,const char * testName, const char * comment);


#endif /* TESTAPP_H_ */
//---- End of source file ------------------------------------------------------
