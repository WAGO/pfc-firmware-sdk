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
///  \file     omsd_config.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#define _GNU_SOURCE 1
#include "omsd.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>


enum eConfigState {
    CONFIG_STATE_GLOBAL = 0,
    CONFIG_STATE_SWITCH,
    CONFIG_STATE_NEW_SWITCH,
    CONFIG_STATE_SCRIPT,
    CONFIG_STATE_ERROR
};

enum eConfigTags {
  CONFIG_TAG_KEY,
  CONFIG_TAG_SWITCH,
  CONFIG_TAG_DOWNTIME,
  CONFIG_TAG_UP,
  CONFIG_TAG_CALL_SCRIPT,
  CONFIG_TAG_CALL_STATE,
  CONFIG_TAG_CALLED,
  CONFIG_TAG_NEED,
  CONFIG_TAG_ENDLIST
};

struct stCallNames {
    char *name;
    struct stCallNames * pNext;
};

typedef struct stSwitches {
    tOmsStateSwitch    * stateSwitch;
    char               * name;
    struct stCallNames * callNames;
    struct stCallNames * needNames;
    struct stCallNames * notNeedNames;
    char               * script;
    bool                 used;
    struct stSwitches  * pNext;
}tSwitches;


typedef struct {
    enum eConfigTags   tagNo;
    char             * tagStr;
    enum eConfigState (*Function)(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch);
}tConfigOpts;

enum  eKeyNames     { RST_ALL=0, RESET};
char * keyNames[] = {"RST_ALL", "RESET", NULL};

enum  eSwNames     { RUN=0, STOP};
char * swNames[] = {"RUN", "STOP", NULL};

struct stCallNames * nameBuffer = NULL;

static int runScript(char * script)
{
    int rc;
    printf("Run Script:\n%s\n", script);
    if (!fork())
    {
      execl("/bin/sh", "sh", "-c", script, NULL);
      exit(1);
    }

    wait(&rc);
    return rc;
}

int isolateVal(char ** val,char ** ppStart)
{
  char *start = *ppStart;
  char *endtag;
  while (isblank(*start) && (*start) != 0)
  {
    if (*start == '\n')
    {
      /*printf("%d argument expected after key\n",
             linenum);*/
      return 1;
    }
    start++;
  }
  if(*start == 0)
  {
    //start is not allowed to be 0 here
    return 2;
  }
  endtag = start;
  while (!isblank(*endtag) && (*endtag) != 0 && (*endtag) != '\n')
  {
    endtag++;
  }
  if(*endtag == 0)
  {
    //if endtag is 0 the syntag of the config file is not correct!
    return 3;
  }
  *endtag = 0;
  *val = start;
  *ppStart = endtag+1;
  return 0;
}

enum eConfigState setKey(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  char *value;
  int i;
  (void)lineNum;

  if(isolateVal(&value,ppStart))
  {
    return CONFIG_STATE_ERROR;
  }

  for(i=0; keyNames[i] != NULL; i++)
  {
    if(!strncmp(value, keyNames[i],strlen(keyNames[i])))
    {
      break;
    }
  }
  actSwitch->stateSwitch->flagsWatch |= OMS_WATCH_MASK;
  switch(i)
  {
    case RST_ALL:
      actSwitch->stateSwitch->MaskWatch |= OMS_WMASK_RSTALL;
      if(notTag)
      {
        actSwitch->stateSwitch->MaskState &= ~(OMS_WMASK_RSTALL);
      }
      else
      {
        actSwitch->stateSwitch->MaskState |= OMS_WMASK_RSTALL;
      }
      break;
    case RESET:
      actSwitch->stateSwitch->MaskWatch |= OMS_WMASK_RESET;
      if(notTag)
      {
        actSwitch->stateSwitch->MaskState &= ~(OMS_WMASK_RESET);
      }
      else
      {
        actSwitch->stateSwitch->MaskState |= OMS_WMASK_RESET;
      }
      break;
    default:
      //FEHLER
      break;
  }

  return CONFIG_STATE_SWITCH;
}

enum eConfigState setSw(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  char *value;
  int i;
  (void)lineNum;

  if(isolateVal(&value,ppStart))
  {
    return CONFIG_STATE_ERROR;
  }

  for(i=0; swNames[i] != NULL; i++)
  {
    if(!strncmp(value, swNames[i],strlen(swNames[i])))
    {
      break;
    }
  }
  actSwitch->stateSwitch->flagsWatch |= OMS_WATCH_MASK;
  switch(i)
  {
    case RUN:
      actSwitch->stateSwitch->MaskWatch |= OMS_WMASK_RUN;
      if(notTag)
      {
        actSwitch->stateSwitch->MaskState &= ~(OMS_WMASK_RUN);
      }
      else
      {
        actSwitch->stateSwitch->MaskState |= OMS_WMASK_RUN;
      }
      break;
    case STOP:
      actSwitch->stateSwitch->MaskWatch |= OMS_WMASK_STOP;
      if(notTag)
      {
        actSwitch->stateSwitch->MaskState &= ~(OMS_WMASK_STOP);
      }
      else
      {
        actSwitch->stateSwitch->MaskState |= OMS_WMASK_STOP;
      }
      break;
    default:
      //FEHLER
      break;
  }

  return CONFIG_STATE_SWITCH;
}

enum eConfigState setUp(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  (void)lineNum;
  (void)ppStart;
  if(notTag)
  {
    actSwitch->stateSwitch->flagsWatch &= ~OMS_WATCH_UP;
  }
  else
  {
    actSwitch->stateSwitch->flagsWatch |= OMS_WATCH_UP;
  }
  return CONFIG_STATE_SWITCH;
}

enum eConfigState setDowntime(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  int timeout;
  (void)lineNum;

  timeout = strtol(*ppStart, ppStart, 10);

  if(notTag)
  {
    timeout = -timeout;
  }
  oms_SM_SetTimeout(actSwitch->stateSwitch,timeout);
  return CONFIG_STATE_SWITCH;
}

enum eConfigState setCalled(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  (void)lineNum;
  (void)ppStart;
  (void)notTag;
  actSwitch->stateSwitch->maschineStateFlags &= ~OMS_MSTATE_ALLWAYS_ACTIVE;
  actSwitch->used = false;
  return CONFIG_STATE_SWITCH;
}

enum eConfigState setNeed(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  (void)lineNum;


  if(nameBuffer != NULL)
  {
    char * pName=NULL;
    if(isolateVal(&pName,ppStart))
    {
      return CONFIG_STATE_ERROR;
    }
    nameBuffer->name = strdup(pName);
    if(notTag)
    {
      nameBuffer->pNext = actSwitch->notNeedNames;
      actSwitch->notNeedNames = nameBuffer;
    }
    else
    {
      nameBuffer->pNext = actSwitch->needNames;
      actSwitch->needNames = nameBuffer;
    }

    nameBuffer = NULL;
  }

  return CONFIG_STATE_SWITCH;
}

enum eConfigState setCallstate(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  (void)lineNum;
  (void)notTag;

  if(nameBuffer != NULL)
  {
    char * pName=NULL;
    if(isolateVal(&pName,ppStart))
    {
      return CONFIG_STATE_ERROR;
    }
    nameBuffer->name = strdup(pName);
    nameBuffer->pNext = actSwitch->callNames;
    actSwitch->callNames = nameBuffer;
    nameBuffer = NULL;
  }

  return CONFIG_STATE_SWITCH;
}

enum eConfigState setCallScript(char ** ppStart, int lineNum, int notTag, tSwitches * actSwitch)
{
  char *start = *ppStart;
  (void)lineNum;
  (void)notTag;

  if(actSwitch->script != NULL)
  {
    puts("error only one script is allowed per switchstate");
    return CONFIG_STATE_ERROR;
  }

 // while (*start != '\n') start++;
  actSwitch->script = start;
  *ppStart = start;

  return CONFIG_STATE_SCRIPT;
}

tConfigOpts cfgOptions[] =
{
 {
  .tagNo    = CONFIG_TAG_KEY,
  .tagStr   = "key",
  .Function = setKey
 },
 {
  .tagNo    = CONFIG_TAG_SWITCH,
  .tagStr   = "switch",
  .Function = setSw
 },
 {
  .tagNo    = CONFIG_TAG_UP,
  .tagStr   = "up",
  .Function = setUp
 },
 {
  .tagNo    = CONFIG_TAG_DOWNTIME,
  .tagStr   = "downtime",
  .Function = setDowntime
 },
 {
  .tagNo    = CONFIG_TAG_CALLED,
  .tagStr   = "called",
  .Function = setCalled
 },
 {
  .tagNo    = CONFIG_TAG_NEED,
  .tagStr   = "need",
  .Function = setNeed
 },
 {
  .tagNo    = CONFIG_TAG_CALL_STATE,
  .tagStr   = "callstate",
  .Function = setCallstate
 },
 {
  .tagNo    = CONFIG_TAG_CALL_SCRIPT,
  .tagStr   = "callscript",
  .Function = setCallScript
 },
 {
  .tagNo    = CONFIG_TAG_ENDLIST
 },
};

enum eConfigState  GlobalConfigs(const char * configFile,char ** ppStart, int lineNum,
                                 char ** namePointer)
{
  enum eConfigState newConfigState = CONFIG_STATE_GLOBAL;
  char * start = *ppStart;
  char * endtag = start;
  while (*endtag != '{' && *endtag != '\0') endtag++;
  if (*endtag != '{')
  {
    printf( "%s:%d missing end of line start: %s endtag: %s\n",
            configFile, lineNum, start , endtag);
  }
  else
  {
    char *firstBlank = start;
    *endtag = '\0';

    while(!isblank(*firstBlank) && *firstBlank != 0)firstBlank++;
    *firstBlank = 0;
    *namePointer = start;

    newConfigState = CONFIG_STATE_NEW_SWITCH;
  }
  *ppStart = endtag + 1;

  return newConfigState;
}

enum eConfigState  SwitchConfigs(char ** ppStart, int lineNum,
                                 tSwitches * actSwitch)
{
  enum eConfigState newConfigState = CONFIG_STATE_SWITCH;
  char * start = *ppStart;
  int notTag = 0;

  if(*start == '}')
  {
    newConfigState = CONFIG_STATE_GLOBAL;
    *ppStart = start + 1;
  }
  else
  {
    int i;
    char * endtag = start;
    while (!isblank(*endtag) && *endtag != '\n' && (*endtag) != 0)
    {
      endtag++;
    }
    *endtag=0;
    if(*start == '!')
    {
      notTag = 1;
      start += 1;
    }
    for(i = 0; cfgOptions[i].tagNo != CONFIG_TAG_ENDLIST; i++)
    {
      if(!strncmp(cfgOptions[i].tagStr, start,strlen(cfgOptions[i].tagStr)))
      {
        start = endtag+1;
        newConfigState = cfgOptions[i].Function(&start, lineNum, notTag, actSwitch);
        endtag = start;
        break;
      }
    }

    if(cfgOptions[i].tagNo == CONFIG_TAG_ENDLIST)
    {
      endtag= endtag+1;
      printf("unknown tag: %s", start);
      newConfigState = CONFIG_STATE_ERROR;
    }
    *ppStart = endtag;
  }
  return newConfigState;
}

enum eConfigState  Scripting(char ** ppStart, int lineNum,
                                 tSwitches * actSwitch)
{
  enum eConfigState newConfigState = CONFIG_STATE_SCRIPT;
  char * start = *ppStart;
  char * endtag = start+1;
  (void)lineNum;
  (void)actSwitch;

    if(!strncmp( start, "endscript",strlen("endscript")))
    {
      char * pScript = actSwitch->script;
      *start = 0;
      actSwitch->script = strdup(pScript);
      newConfigState = CONFIG_STATE_SWITCH;
    }
    while (*endtag != '\n') endtag++;
    *ppStart = endtag;
  return newConfigState;
}

int CheckSwitchConnections(tSwitches * switchRoot)
{
  tSwitches * pAct = switchRoot;
  tSwitches * pSearch = NULL;

  while(pAct != NULL)
  {
    //duplications
    pSearch = pAct->pNext;
    while(pSearch != NULL)
    {
      if(!strcmp(pAct->name, pSearch->name))
      {
        printf("error name %s exist two times!\n", pAct->name);
        return 1;
      }
      pSearch = pSearch->pNext;
    }

    //callStates
    if(pAct->callNames != NULL)
    {
      struct stCallNames * pActName = pAct->callNames;
      while(pActName != NULL)
      {
        pSearch = switchRoot;
        while(pSearch != NULL)
        {
          if(!strcmp(pSearch->name, pActName->name))
          {
            pSearch->used = true;
            break;
          }
          pSearch = pSearch->pNext;
        }
        if(pSearch == NULL)
        {
          printf("error Stateswitch %s called by %s does not exist\n", pActName->name, pAct->name);
          return 1;
        }
        pActName = pActName->pNext;
      }
    }

    //needStates
    if(pAct->needNames != NULL)
    {
      struct stCallNames * pActName = pAct->needNames;
      while(pActName != NULL)
      {
        pSearch = switchRoot;
        while(pSearch != NULL)
        {
          if(!strcmp(pSearch->name, pActName->name))
          {
            break;
          }
          pSearch = pSearch->pNext;
        }
        if(pSearch == NULL)
        {
          printf("error Stateswitch %s needed by %s does not exist\n", pActName->name, pAct->name);
          return 1;
        }
        pActName = pActName->pNext;
      }
    }

    //notNeedStates
    if(pAct->notNeedNames != NULL)
    {
      struct stCallNames * pActName = pAct->notNeedNames;
      while(pActName != NULL)
      {
        pSearch = switchRoot;
        while(pSearch != NULL)
        {
          if(!strcmp(pSearch->name, pActName->name))
          {
            break;
          }
          pSearch = pSearch->pNext;
        }
        if(pSearch == NULL)
        {
          printf("error StateSwitch %s exluded by %s does not exist\n", pActName->name, pAct->name);
          return 1;
        }
        pActName = pActName->pNext;
      }
    }
    pAct = pAct->pNext;
  }

  return 0;
}

void freeStateSwitchList(tOmsStateSwitch * stateSwitch)
{
  /*
   * this entries are unused an so not to be freed in this case
  tOmsStateCallList       * callStates;
  tOmsStateCallList       * callNotActive;
  tOmsStateCallList       * callNeedActive;
  struct stOmsStateSwitch * pNext;*/
  while(stateSwitch != NULL)
  {
    tOmsStateSwitch * pDel = stateSwitch;
    stateSwitch = stateSwitch->pNext;
    free(pDel);
  }
}

void Memorize(tSwitches * switchRoot)
{
  tSwitches * pAct = switchRoot;
  while(pAct)
  {
    if(pAct->used != true)
    {
      printf("warning: Switchstate %s is defined but not used!\n", pAct->name);
    }
    else
    {
      tOmsStateSwitch *pNew = oms_SM_NewStateSwitch(pAct->stateSwitch->maschineStateFlags & OMS_MSTATE_ALLWAYS_ACTIVE);
      if(pNew != NULL)
      {
        pNew->MaskState  = pAct->stateSwitch->MaskState;
        pNew->MaskWatch  = pAct->stateSwitch->MaskWatch;
        pNew->flagsState = pAct->stateSwitch->flagsState;
        pNew->flagsWatch = pAct->stateSwitch->flagsWatch;
        pNew->timeoutMsec = pAct->stateSwitch->timeoutMsec;
      }
      freeStateSwitchList(pAct->stateSwitch);
      pAct->stateSwitch = pNew;
    }
    pAct = pAct->pNext;
  }
}

void Connect(tSwitches * switchRoot)
{
  tSwitches * pAct = switchRoot;
  while(pAct)
  {
    struct stCallNames * pConnect;
    tSwitches *pSearch;
    if(pAct->used == true)
    {
      //CallStates
      pConnect = pAct->callNames;
      while(pConnect != NULL)
      {
        pSearch = switchRoot;
        while(pSearch != NULL)
        {
          if(!strcmp(pConnect->name, pSearch->name))
          {
            oms_SM_SetFollowSwitch(pAct->stateSwitch,pSearch->stateSwitch);
            break;
          }
          pSearch = pSearch->pNext;
        }
        pConnect = pConnect->pNext;
      }

      //NeedStates
      pConnect = pAct->needNames;
      while(pConnect != NULL)
      {
        pSearch = switchRoot;
        while(pSearch != NULL)
        {
          if(!strcmp(pConnect->name, pSearch->name))
          {
            oms_SM_SetNeedActiveSwitch(pAct->stateSwitch,pSearch->stateSwitch);
            break;
          }
          pSearch = pSearch->pNext;
        }
        pConnect = pConnect->pNext;
      }

      //NotNeedStates
      pConnect = pAct->notNeedNames;
      while(pConnect != NULL)
      {
        pSearch = switchRoot;
        while(pSearch != NULL)
        {
          if(!strcmp(pConnect->name, pSearch->name))
          {
            oms_SM_SetNotActiveSwitch(pAct->stateSwitch,pSearch->stateSwitch);
            break;
          }
          pSearch = pSearch->pNext;
        }
        pConnect = pConnect->pNext;
      }
    }
    pAct = pAct->pNext;
  }
}
void Scriptorize(tSwitches * switchRoot)
{
  tSwitches * pAct = switchRoot;
  while(pAct)
  {

    if(pAct->used == true)
    {

      if(pAct->script != NULL)
      {

        char * script = strdup(pAct->script);

        oms_SM_SetCallFunction(pAct->stateSwitch, (tOmsEventfunction)runScript,(void*) script);
      }
    }

    pAct = pAct->pNext;

  }
}

void RegisterSwitches(tSwitches * switchRoot)
{
  tSwitches * pAct = switchRoot;
  while(pAct)
  {
    if(pAct->used == true)
    {
      oms_SM_RegisterSwitch(pAct->stateSwitch);
    }
    pAct = pAct->pNext;
  }
}

int ConfigurateSwitches(tSwitches * switchRoot)
{
  if (CheckSwitchConnections(switchRoot))
  {
    return 1;
  }
  Memorize(switchRoot);
  Connect(switchRoot);
  Scriptorize(switchRoot);
  RegisterSwitches(switchRoot);

  return 0;
}

int importFile(const char * configFile,tSwitches ** switchRoot, tSwitches ** actSwitch)
{
  int fd;
  char * buf = NULL;
  int length;
  int lineNum = 1;
  struct stat sb;
  char * start;
  char *namePointer;
  int ret = 0;

  enum eConfigState configState = CONFIG_STATE_GLOBAL;

  fd = open(configFile, O_RDONLY);
  if (fd < 0)
  {
    printf( "failed to open config file %s: %s\n",
    configFile, strerror(errno));
    ret = 1;
    goto functionExit;
  }

  if (fstat(fd, &sb))
  {
    printf( "fstat of %s failed: %s\n", configFile,
    strerror(errno));
    close(fd);
    ret=1;
    goto functionExit;
  }
  if (!S_ISREG(sb.st_mode))
  {
    printf( "Ignoring %s because it's not a regular file.\n",
    configFile);
    close(fd);
    ret=0;
    goto functionExit;
  }

  length = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  buf = malloc(length + 2);
  if (!buf)
  {
    printf( "malloc() of %d bytes failed\n", length);
    close(fd);
    ret=1;
    goto functionExit;
  }

  if (read(fd, buf, length) != length)
  {
    printf( "failed to read %s: %s\n", configFile,
    strerror(errno));
    close(fd);
    ret = 1;
    goto functionExit;
  }

  close(fd);

  /* knowing the buffer ends with a newline makes things (a bit) cleaner */
  buf[length + 1] = '\0';
  buf[length] = '\n';


  start = buf;
  while (*start)
  {
    //usleep(50000);
    while (isblank(*start) && (*start))
    {
      start++;
    }
    if (*start == '#')
    {
      while (*start != '\n')
      {
       start++;
      }
    }

    if (*start == '\n')
    {
      start++;
      lineNum++;
      continue;
    }

    if(nameBuffer == NULL)
    {
      nameBuffer = malloc(sizeof(struct stCallNames));
      nameBuffer->name  = NULL;
      nameBuffer->pNext = NULL;
    }
    switch(configState)
    {
      case CONFIG_STATE_GLOBAL:
        configState = GlobalConfigs(configFile, &start, lineNum, &namePointer);
        break;
      case CONFIG_STATE_SWITCH:
        configState = SwitchConfigs(&start, lineNum, *actSwitch);
        break;
      case CONFIG_STATE_SCRIPT:
        configState = Scripting(&start, lineNum, *actSwitch);
        break;
      case CONFIG_STATE_NEW_SWITCH:
        if( *switchRoot == NULL)
        {
         (*switchRoot) = malloc(sizeof(tSwitches));
         (*actSwitch) = (*switchRoot);
        }
        else
        {
         (*actSwitch)->pNext = malloc(sizeof(tSwitches));
         (*actSwitch) = (*actSwitch)->pNext;
        }
        (*actSwitch)->stateSwitch = malloc(sizeof(tOmsStateSwitch));
        memset((*actSwitch)->stateSwitch, 0 ,sizeof(tOmsStateSwitch));
        (*actSwitch)->stateSwitch->maschineStateFlags |=  OMS_MSTATE_ALLWAYS_ACTIVE;
        (*actSwitch)->name = strdup(namePointer);
        (*actSwitch)->used = true;
        (*actSwitch)->callNames = NULL;
        (*actSwitch)->needNames = NULL;
        (*actSwitch)->notNeedNames = NULL;
        (*actSwitch)->script = NULL;
        (*actSwitch)->pNext = NULL;
        configState = CONFIG_STATE_SWITCH;
        break;
      case CONFIG_STATE_ERROR:
        ret = 1;
        goto functionExit;
        break;
      default:

        break;
    }
  }

  free(buf);
  return 0;


  functionExit:
  if(buf != NULL)
  {
    free(buf);
  }
  if(nameBuffer != NULL)
  {
    free(nameBuffer);
  }
  return ret;
}

int confFilter(const struct dirent * pDir)
{
  char * pDot = strrchr(pDir->d_name,'.');

  if(pDot != NULL)
  {
    pDot++;
    if((*pDot != 0) && (!strcmp(pDot,"conf")))
    {
      return 1;
    }
  }

  return 0;
}

void freeNameList(struct stCallNames * nameList)
{
  while(nameList != NULL)
  {
    struct stCallNames * pDel = nameList;
    nameList = nameList->pNext;
    free(pDel->name);
    free(pDel);
  }
}

void freeSwitchList(tSwitches * switchRoot)
{
  while(switchRoot != NULL)
  {
    tSwitches * pDel = switchRoot;
    switchRoot = switchRoot->pNext;
    //This is used by another object at this time dont free it
    //pDel->stateSwitch
    free(pDel->name);
    freeNameList(pDel->callNames);
    freeNameList(pDel->needNames);
    freeNameList(pDel->notNeedNames);
    if(pDel->script != NULL)
    {
      free(pDel->script);
    }
    free(pDel);
  }
}

int readConfigFile(const char * configFile)
{
  tSwitches * switchRoot = NULL, * actSwitch = NULL;
  int ret = 1;
  if(!importFile(configFile,&switchRoot,&actSwitch))
  {
    ret =  ConfigurateSwitches(switchRoot);
  }

  freeSwitchList(switchRoot);
  return ret;
}

int readConfigDir(const char * configDir)
{
  tSwitches * switchRoot = NULL, * actSwitch = NULL;
  struct dirent **namelist;
  int n;
  int ret = 0;

  n = scandir(configDir, &namelist, confFilter, alphasort);

  if(n > 0)
  {
    for(n--;n>=0;n--)
    {
      char pathName[1024];
      sprintf(pathName,"%s/%s",configDir,namelist[n]->d_name);

      if(importFile(pathName,&switchRoot,&actSwitch))
      {
        ret = 1;
      }
      free(namelist[n]);
    }
    free(namelist);
  }
  else
  {
    if(n < 0)
    {
      perror("scandir");
    }
    ret = 1;
  }
  if(ret == 0)
  {
    ret = ConfigurateSwitches(switchRoot);
  }
  freeSwitchList(switchRoot);

  return ret;
}

//---- End of source file ------------------------------------------------------
