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
///  \file     auto.c
///
///  \version  $Rev: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "testapp.h"
#include "ledmisc.h"
#include <diagnostic/diagnostic_API.h>

#include <unistd.h>



static int idFireingValid()
{
  puts("Sende valide IDs systematisch");

  puts("generiere xml-datei");
  GenerateValidOnlyXmlFile();
  puts("warte 5 sekunden, damit der LED-Server die neue Datei übernimmt");
  sleep(5);
  puts("beginne mit dem senden der IDs");

  return sendAllIds(100000);
}

static int idFireingInvalid()
{
  puts("Sende invalide IDs systematisch");

  puts("generiere xml-datei");
  GenerateInvalidOnlyXmlFile();
  puts("warte 5 sekunden, damit der LED-Server die neue Datei übernimmt");
  sleep(5);
  puts("beginne mit dem senden der IDs");

  return sendAllIds(100000);
}

static int idFireing()
{
  puts("Sende alle IDs systematisch");

  puts("generiere xml-datei");
  GenerateXmlFile();
  puts("warte 5 sekunden, damit der LED-Server die neue Datei übernimmt");
  sleep(5);
  puts("beginne mit dem senden der IDs");

  return sendAllIds(100000);
}



tTestCase autoTests[] =
{
 { .name="fireValid", .help="alle verfügbaren validen IDs nacheinander in systematischer kombinatorik senden" , .function=idFireingValid},
 { .name="fireInvalid", .help="alle verfügbaren invaliden IDs nacheinander in systematischer kombinatorik senden" , .function=idFireingInvalid},
 { .name="fire", .help="alle verfügbaren IDs nacheinander in systematischer kombinatorik senden" , .function=idFireing},
 { .name="exit",        .help="menü verlassen" ,                      .function=TestExit},
};

size_t numberOfAutoTests = sizeof(autoTests)/sizeof(tTestCase);

//---- End of source file ------------------------------------------------------
