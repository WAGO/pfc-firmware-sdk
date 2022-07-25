//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
/*
 * parse_log.c
 *
 *  Created on: 02.06.2012
 *      Author: flormel
 */
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "parse_log.h"

#define TIME_FORMAT "%a %b %d %Y %T"

void FreeLogData(tLogData * del)
{
  if(del != NULL)
  {
    free(del->progName);
    free(del->variables);
    free(del);
  }
}


tLogData * ParseLogLine(const char * logLine)
{
  struct tm timestamp;
  char *p;
  tLogData * logData = malloc(sizeof(tLogData));
  char buffer[4096];
  int i;
  p = strptime(logLine, TIME_FORMAT, &timestamp);
  logData->timestamp.tv_sec = mktime(&timestamp);
  p++;

  logData->timestamp.tv_usec = strtol(p,&p,10);
  p++;

  logData->id = strtoll(p,&p,16);
  p++;

  for(i = 0; i < 4096; i++)
  {
    buffer[i] = *(p++);
    if(buffer[i]== ':')
    {
      buffer[i] = 0;
      i++;
      break;
    }
  }
  logData->progName = malloc(sizeof(char) * i+1);
  strncpy(logData->progName, buffer, i);
  logData->progName[i] = 0;
  p++;

  if(*p == 'S')
  {
    logData->set = true;
  }
  else
  {
    logData->set = false;
  }
  p++;
  i  = strlen(p);
  if(i > 2)
  {
    logData->variables = malloc(sizeof(char) * i+1);
    strncpy(logData->variables, p, i);
    logData->variables[i] = 0;
  }
  else
  {
    logData->variables = NULL;
  }
  return logData;
}


bool       CheckLineForId(uint32_t id, const char * logLine)
{
  char *p = (char *) (logLine + 32);
  if(id == (uint32_t) strtol(p, NULL, 16))
  {
    return true;
  }
  return false;
}

char * nextWord(char *p)
{
  while(isgraph(*(p)))
  {
    p++;
  }
  while(!isgraph(*(p)))
  {
    if(*p == '\n')
    {
      return NULL;
    }
    p++;
  }
  return p;
}

//was die funktion macht ist totaler quatsch
int GetVarString(char * dest,  char *src, char **endptr,tLogData * logData)
{
  char * p = (char * )src;
  int ret = 0;
  int varnum = 0;
  char printtype = 'd';

  if(*p != '$')
  {
    return 0;
  }
  p++;
  if(*p == '(')
  {
    p++;
    if(isalpha(*p))
    {
      printtype=*p;
      p++;
    }
    varnum =strtoll(p,&p,10);
    p++;
  }
  else
  {
    if(isalpha(*p))
    {
      printtype=*p;
      p++;
    }
    varnum =strtoll(p,&p,10);
  }
  if(varnum == 0)
  {
    goto errorOut;
  }
  *endptr = p;
  if(logData->variables == NULL)
  {
    goto errorOut;
  }
  else
  {
    int varcount;
    p = logData->variables;

    while(!isgraph(*(p)))
    {
      p++;
    }
    for(varcount = 1; varcount < varnum; varcount++)
    {
      p = nextWord(p);
      if(p == NULL)
      {
        goto errorOut;
      }
      p = nextWord(p);
      if(p == NULL)
      {
        goto errorOut;
      }
    }

    p = nextWord(p);
    if(p == NULL)
    {
      goto errorOut;
    }

    if(printtype != 'd')
    {
      char format[16];
      uint64_t val = strtoll(p, NULL, 10);
      sprintf(format, "%%%c", printtype);
      ret = sprintf(dest, format, val);
    }
    else
    {
      while(isgraph(p[ret]))
      {
        if(   (*p == '\n')
           || (*p == '\0'))
        {
          break;
        }
        dest[ret]= p[ret];
        ret++;
      }
    }
  }


  return ret;
errorOut:
  *(dest++) = 'E';
  *(dest++) = 'R';
  *(dest++) = 'R';
  return 3;

}

void ReplaceVariables(char * dest, char *src, tLogData * logData, size_t destSize)
{
  int i;

  for(i = 0; i < (int)destSize; i++)
  {
    if(*src == '$')
    {
      i += GetVarString(&dest[i], src, &src, logData);
      i--;
      continue;
    }
    dest[i] = *(src++);
    if(    (!isprint(dest[i]))
        && (dest[i] != '\n'))
    {
      dest[i] = 0;
      break;
    }
  }
}

const char * MergeLog(const char * logDef, tLogData * logData, bool resetStr)
{
  static char buffer[4096];
  char       * pBuffer;
  int i;
  //char *p = (char*) logDef;


  i = strftime(buffer, 64, TIME_FORMAT, localtime(&logData->timestamp.tv_sec));
  pBuffer = buffer + i;
  sprintf(pBuffer, ".%.6d %s: " ,(int) logData->timestamp.tv_usec, logData->progName);
  if(resetStr == false)
  {
    if(logData->set == true)
    {
      strcat(pBuffer, "");
    }
    else
    {
      strcat(pBuffer, "Event reset:");
    }
  }
  pBuffer += strlen(pBuffer);
  ReplaceVariables(pBuffer,(char*)logDef, logData,sizeof(buffer) - strlen(buffer));


  return (const char *)buffer;
}
