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
///  \file     interactive.c
///
///  \version  $Rev: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "testapp.h"
#include "ledmisc.h"

#include <unistd.h>




tTestCase interactiveTests[] =
{
// { .name="xml",        .help="generiere Test-XML-datei" , .function=GenerateXmlFile},
 { .name="exit",       .help="menü verlassen" ,           .function=TestExit},
};

size_t numberOfInteractiveTests = sizeof(interactiveTests)/sizeof(tTestCase);



//---- End of source file ------------------------------------------------------
