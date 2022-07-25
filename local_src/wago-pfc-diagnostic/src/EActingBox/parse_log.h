//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
/*
 * parse_log.h
 *
 *  Created on: 02.06.2012
 *      Author: flormel
 */

#ifndef PARSE_LOG_H_
#define PARSE_LOG_H_

#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint32_t         id;
  struct timeval   timestamp;
  char           * progName;
  bool             set;
  char           * variables;
}tLogData;

void FreeLogData(tLogData * del);
tLogData * ParseLogLine(const char * logLine);
bool       CheckLineForId(uint32_t id, const char * logLine);
void ReplaceVariables(char * dest, char *src, tLogData * logData, size_t destSize);
const char * MergeLog(const char * logDef, tLogData * logData, bool resetStr);

#endif /* PARSE_LOG_H_ */
