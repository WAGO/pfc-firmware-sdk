// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "string.h"
#include "testutil_diagnostic.h"

static char programNameArray[64];
static char *programName = NULL;
constexpr auto MAX_EVENT_INDEX = 64;
static log_tEventId eventIdArray[MAX_EVENT_INDEX]; //FIFO array for Event-IDs
static bool eventSetArray[MAX_EVENT_INDEX]; //FIFO array for Event-States
static size_t eventIndexWrite = 0;
static size_t eventIndexRead = 0;

extern "C"
{

void test_log_EVENT_init()
{
  for (int i=0;i<MAX_EVENT_INDEX;i++)
  {
    eventIdArray[i] = 0;
    eventSetArray[i] = false;
  }
}

void log_EVENT_Init(const char *name)
{
    strncpy(programNameArray, name, sizeof(programNameArray));
    programNameArray[sizeof(programNameArray)-1]='\0';
    programName = (char*)programNameArray;
}

void log_EVENT_LogId(log_tEventId id,
                     bool set)
{
    size_t eventIndexNext = eventIndexWrite + 1;

    if (eventIndexNext == MAX_EVENT_INDEX)
    {
      eventIndexNext = 0;
    }

    if (eventIndexNext != eventIndexRead)
    {
      eventIdArray[eventIndexWrite] = id;
      eventSetArray[eventIndexWrite] = set;
      eventIndexWrite = eventIndexNext;
    }
}

void log_EVENT_LogIdParam(log_tEventId id,
                          bool set,
                          int  first_arg_type, ...)
{
    (void)first_arg_type; //todo: extend test for diagnostic variables
    log_EVENT_LogId(id, set);
}

const char* test_get_log_EVENT_ProgramName()
{
  return programName;
}

bool test_log_EVENT_LogId_get_first(log_tEventId *id, bool *set)
{
  if (eventIndexRead == eventIndexWrite)
  { //FIFO empty
    return false;
  }
  *id = eventIdArray[eventIndexRead];
  *set = eventSetArray[eventIndexRead];
  eventIndexRead = eventIndexRead + 1;

  if (eventIndexRead == MAX_EVENT_INDEX)
  {
    eventIndexRead = 0;
  }
  return true;
}

} //extern "C"
//---- End of source file ------------------------------------------------------


