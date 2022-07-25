//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     get_rts_info.c
///
///  \brief    Read information about CoDeSys 2 PLC.
///
///  \author   SM:  WAGO GmbH & Co. KG
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

#include <glib.h>

#include "wago_ct_get_rts_info.h"
#include "wago_ct_ipc_msg_com.h"

#include "config_tool_lib.h"
#include "config_tool_msg_com.h"

#ifdef  __CPPUTEST__ 
#define main get_rts_info_main
#define ShowHelpText get_rts_info_ShowHelpText
#endif

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define TIMEOUT_RECEIVE_ANSWER                                                 2

// maximum length of ouput string - should be long enough for all values (-> project description!)
#define MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO                                256

#define CTRL_TASK_MILLIFACTOR                                               1000

#define CDS2_BOOT_PROJECT_FILE                       "/home/codesys/DEFAULT.PRG"
#define CDS2_CHECKSUM_FILE                           "/home/codesys/DEFAULT.CHK"

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


void GetTaskStatusString(int    taskStatus,
                         char*  psnTaskStatus)
{
  switch(taskStatus)
  {
    case 0:   strncpy(psnTaskStatus, "RUN",             MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 1:   strncpy(psnTaskStatus, "STOP",            MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 2:   strncpy(psnTaskStatus, "STOP BREAKPOINT", MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 3:   strncpy(psnTaskStatus, "RUN STEP IN",     MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 4:   strncpy(psnTaskStatus, "RUN STEP OVER",   MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 5:   strncpy(psnTaskStatus, "RUN WATCHDOG",    MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 6:   strncpy(psnTaskStatus, "STOP WATCHDOG",   MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 7:   strncpy(psnTaskStatus, "STOP ERROR",      MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 8:   strncpy(psnTaskStatus, "SINGLE CYCLE",    MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 9:   strncpy(psnTaskStatus, "STOP BP SWITCH",  MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 10:  strncpy(psnTaskStatus, "NOT CONFIGURED",  MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    default:  strncpy(psnTaskStatus, "ILLEGAL/UNKNOWN", MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
  }
  return;
}


void GetTaskModeString(int    taskMode,
                       char*  psnTaskMode)
{
  switch(taskMode)
  {
    case 0:   strncpy(psnTaskMode, "UNHANDLED",       MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 1:   strncpy(psnTaskMode, "CONTINUE",        MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 2:   strncpy(psnTaskMode, "SUSPEND",         MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 3:   strncpy(psnTaskMode, "EXIT",            MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    case 4:   strncpy(psnTaskMode, "STOP",            MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
    default:  strncpy(psnTaskMode, "ILLEGAL/UNKNOWN", MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO); break;
  }
  return;
}


void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  printf("\n* Get informations about the PLC; state, project info and task infos *\n\n");
  printf("Usage: get_rts_infos <parameter area> [task number] [parameter]\n\n");
  printf("parameter area: \"state\" | \"project\" | \"task\" | \"task-list\" | \"task-list-json\" | \"nr-of-tasks\"\n");
  printf("                task-list returns the html-code for the task list table on webpage\n");
  printf("                task-list-json returns all task infos in json format\n");
  printf("\n");
  printf("task number: only necessary, at parameter area \"task\"\n");
  printf("\n");
  printf("parameter: depending on the specified parameter area, following parameters are possible:\n");
  printf("           state or task-list: no parameter necessary\n");
  printf("           project: \"date\" | \"title\" | \"version\" | \"author\" | \"description\" | \"checksum\"\n");
  printf("           task: \"name\" | \"id\" | \"count\" | \"cycletime\" | \"cycletime-min\" | \"cycletime-max\" |\n");
  printf("           \"cycletime-acc\" |\"status\" | \"mode\" | \"priority\" | \"interval\" | \"interval-unit\" | \"event\" |\n");
  printf("           \"function-pointer\" | \"function-index\"\n");
  printf("\n");
}


int GetPlcState(char *pOutputString)
//
// Get PLC state by message communication and write it to string.
//
{
  int status = SUCCESS;

  tstMsgPlcState stMsgPlcState;

  status = ipcMsgCom_Send(ipcMsgCom_PLC_STATE, (void*)&stMsgPlcState, sizeof(stMsgPlcState), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);
  if(SUCCESS == status)
  {
    char  snStatus[MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO];
    snStatus[0] = 0;
    GetTaskStatusString(stMsgPlcState.state, snStatus);
    snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%s", snStatus);
  }

  return status;
}

// CoDeSys uses Windows CP1251 Codepage which is a flavour of ISO-8859-1
static gchar* AnsiToUtf8(char *strAnsi, char *strUtf8, size_t strUtf8Len)
{
  GError *error = NULL;
  
  gchar *result = g_convert(strAnsi, -1, "UTF-8", "ISO-8859-1", NULL, NULL, &error);

  if(NULL == error)
  {
    snprintf(strUtf8, strUtf8Len, "%s", result);
  }
  else
  {
    snprintf(strUtf8, strUtf8Len, "%s", "Error reading value");
    g_error_free(error);
  }

  g_free(result);

  return strUtf8;
}

int ShowProjectInfo(char* psnParameterName,
                    char *pOutputString)
//
// Get several data of project info from PLC by message communication and write it to string.
//
{
  int status = SUCCESS;

  tstMsgProjectInfo stMsgProjectInfo;

  if(psnParameterName == NULL)
  {
    return(INVALID_PARAMETER);
  }

  status = ipcMsgCom_Send(ipcMsgCom_PROJECT_INFO, (void*)&stMsgProjectInfo, sizeof(stMsgProjectInfo), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);
  if(SUCCESS == status)
  {
    if((strcmp(psnParameterName, "date") == 0) && (stMsgProjectInfo.DTDATE > 0))
    {
      char        snTime[]    = "0000-00-00 00:00:00";
      struct tm*  pstTmTime   = NULL;

      pstTmTime = (struct tm*)gmtime((time_t*)&stMsgProjectInfo.DTDATE);
      strftime(snTime, sizeof(snTime), "%Y-%m-%d %H:%M:%S", pstTmTime);
      snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%s", snTime);
    }

    else if(strcmp(psnParameterName, "title") == 0)
    {
      AnsiToUtf8(stMsgProjectInfo.STTITLE, pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO);
    }
    else if(strcmp(psnParameterName, "version") == 0)
    {
      AnsiToUtf8(stMsgProjectInfo.STVERSION, pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO);
    }
    else if(strcmp(psnParameterName, "author") == 0)
    {
      AnsiToUtf8(stMsgProjectInfo.STAUTHOR, pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO);
    }
    else if(strcmp(psnParameterName, "description") == 0)
    {
      // description is too large for ouput string - write it directly to stdout
      //snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%s", stMsgProjectInfo.STDESCRIPTION);
      GError *error = NULL;
      gchar *descr = g_convert(stMsgProjectInfo.STDESCRIPTION, -1, "UTF-8", "ISO-8859-1", NULL, NULL, &error);
    
      if(NULL == error)
      {
        printf("%s", descr);
      }
      else
      {
        status = ERROR;
        fprintf(stderr, "%s", "Error reading value");
        g_error_free(error);
      }

      g_free(descr);
    }
    else if((strcmp(psnParameterName, "checksum") == 0) && (stMsgProjectInfo.DTDATE > 0))
    {
      char const szChecksumError[] = "Checksum not available:";
      if(access(CDS2_BOOT_PROJECT_FILE, F_OK) < 0)
      {
        // Return replacement value when no boot project is present (=> no checksum available)
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%s%s", szChecksumError, " no boot project");
      }
      else
      {
        FILE *pChecksumFile = fopen(CDS2_CHECKSUM_FILE, "r");
        if(pChecksumFile == NULL)
        {
          status = FILE_OPEN_ERROR;
          fprintf(stderr, "%s %s\n", szChecksumError, strerror(errno));
          SetLastError(strerror(errno));
        }
        else
        {
          uint32_t checksum = 0;
          size_t readElements = fread(&checksum, 4U, 1U, pChecksumFile);
          fclose(pChecksumFile);
          if(readElements != 1U)
          {
            status = FILE_READ_ERROR;
            char const szChecksumReadError[] = "Failed to read checksum";
            fprintf(stderr, "%s %s\n", szChecksumError, szChecksumReadError);
            SetLastError(szChecksumReadError);
          }
          else
          {
            snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%u", checksum);
          }
        }
      }
    }
    // else - unknown parameter -> do nothing
  }

  return status;
}


int ShowTaskInfo(char* psnTaskNo, 
                 char* psnParameterName,
                 char* pOutputString)
//
// Get several data of the rts task from PLC by message communication and write it to string.
//
{
  int   status    = SUCCESS;
  int   taskNo    = 0;
  //printf("ShowTaskInfo\n");

  if((psnTaskNo == NULL) || (psnParameterName == NULL))
  {
    return -1;
  }

  sscanf(psnTaskNo, "%d", &taskNo);
  if(taskNo < 0)
  {
    status = -1;
  }
  else
  {
    tstMsgTaskInfo stMsgTaskInfo;

    stMsgTaskInfo.taskNo = taskNo;
    status = ipcMsgCom_Send(ipcMsgCom_TASK_INFO, (void*)&stMsgTaskInfo, sizeof(stMsgTaskInfo), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);

    if(SUCCESS == status)
    {
      //printf("taskno:%d\n", stMsgTaskInfo.taskNo);
      if(stMsgTaskInfo.status != SUCCESS)
      {
        status = NOT_FOUND;
      }

      else if(strcmp(psnParameterName, "number") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.numberOfTasks);
      }
      else if(strcmp(psnParameterName, "name") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%s", stMsgTaskInfo.stTaskDownloadFormat.cName);
      }
      else if(strcmp(psnParameterName, "id") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskDescription.ulTaskId);
      }
      else if(strcmp(psnParameterName, "count") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskRtInfo.ulCount);
      }
      else if(strcmp(psnParameterName, "cycletime") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskRtInfo.ulCycleTime);
      }

      else if(strcmp(psnParameterName, "cycletime-min") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "-");
        if(stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMin != (unsigned long)-1)
        {
          snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMin);
        }
      }

      else if(strcmp(psnParameterName, "cycletime-max") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMax);
      }

      else if(strcmp(psnParameterName, "cycletime-acc") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "-");
        if((stMsgTaskInfo.stTaskRtInfo.ulCount > 0) && (stMsgTaskInfo.stTaskRtInfo.ulCycleTimeAcc != -1))
        {
          snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", 
                   stMsgTaskInfo.stTaskRtInfo.ulCycleTimeAcc/stMsgTaskInfo.stTaskRtInfo.ulCount);
        }
      }

      else if(strcmp(psnParameterName, "status") == 0)
      {
        GetTaskStatusString(stMsgTaskInfo.stTaskRtInfo.uStatus, pOutputString);
      }

      else if(strcmp(psnParameterName, "mode") == 0)
      {
        GetTaskModeString(stMsgTaskInfo.stTaskRtInfo.uMode, pOutputString);
      }

      else if(strcmp(psnParameterName, "priority") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskDownloadFormat.ulPriority);
      }

      else if(strcmp(psnParameterName, "interval") == 0)
      {
        long interval     = stMsgTaskInfo.stTaskDownloadFormat.lInterval;
        int  taskTimebase = stMsgTaskInfo.stTaskDownloadFormat.taskTimebase; 

        if((taskTimebase == CTRL_TASK_MILLIFACTOR) || ((taskTimebase == 1) && (interval % 1000 == 0)) )
        {
          interval = interval / 1000;
        }
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", interval);
      }

      else if(strcmp(psnParameterName, "interval-unit") == 0)
      {
        char snUnit[]     = "mcsec";
        long interval     = stMsgTaskInfo.stTaskDownloadFormat.lInterval;
        int  taskTimebase = stMsgTaskInfo.stTaskDownloadFormat.taskTimebase; 

        if((taskTimebase == CTRL_TASK_MILLIFACTOR) || ((taskTimebase == 1) && (interval % 1000 == 0)) )
        {
          strncpy(snUnit, "msec", sizeof(snUnit));
        }
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%s", snUnit);
      }

      else if(strcmp(psnParameterName, "event") == 0)
      {
        if(stMsgTaskInfo.stTaskDownloadFormat.usEventRefid == (unsigned short)-1)
        {
          snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "NONE");
        }
        else
        {
          snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d:%d:%d",
                   stMsgTaskInfo.stTaskDownloadFormat.usEventRefid, 
                   stMsgTaskInfo.stTaskDownloadFormat.ulEventOffset,
                   stMsgTaskInfo.stTaskDownloadFormat.usEventSize);
        }
      }

      else if(strcmp(psnParameterName, "function-pointer") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "0x%08x", stMsgTaskInfo.stTaskDescription.pfTask);
      }
      else if(strcmp(psnParameterName, "function-index") == 0)
      {
        snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO, "%d", stMsgTaskInfo.stTaskDownloadFormat.ulPOUIndex);
      }
    }
  }

  return status;
}



int ShowTaskInfoList(void)
//
// Create the html-code for a table to show the infos about the actual existing tasks in RTS.
// Get the data by message communication with PLC.
//
{
  int             status          = SUCCESS;
  int             taskNo          = 0;
  tstMsgTaskInfo  stMsgTaskInfo;
  
  stMsgTaskInfo.taskNo = 0;
  status = ipcMsgCom_Send(ipcMsgCom_TASK_INFO, (void*)&stMsgTaskInfo, sizeof(stMsgTaskInfo), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);

  if(status == SUCCESS)
  {
    printf("        <td>Number of tasks</td>\n");
    printf("        <td>%d</td>\n", stMsgTaskInfo.numberOfTasks);
  }

  while((SUCCESS == status) && (stMsgTaskInfo.status == SUCCESS))
  {
    char snStatus[MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO]   = "";
    char snMode[MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO]     = "";
    char snUnit[]                             = "mcsec";

    long interval     = stMsgTaskInfo.stTaskDownloadFormat.lInterval;
    int  taskTimebase = stMsgTaskInfo.stTaskDownloadFormat.taskTimebase; 

    GetTaskStatusString(stMsgTaskInfo.stTaskRtInfo.uStatus, snStatus);
    GetTaskModeString(stMsgTaskInfo.stTaskRtInfo.uMode, snMode);

    if((taskTimebase == CTRL_TASK_MILLIFACTOR) || ((taskTimebase == 1) && (interval % 1000 == 0)) )
    {
      strncpy(snUnit, "msec", sizeof(snUnit));
    }

    if((taskTimebase == CTRL_TASK_MILLIFACTOR) || ((taskTimebase == 1) && (interval % 1000 == 0)) )
    {
      interval = interval / 1000;
    }

    printf("        <tr>\n");
    printf("          <td colspan=\"2\" class=\"subheading\">Task %d: %s, ID: %d</td>\n",
                      taskNo, stMsgTaskInfo.stTaskDownloadFormat.cName, stMsgTaskInfo.stTaskDescription.ulTaskId);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Cycle count</td>\n");
    printf("          <td>%d</td>\n", stMsgTaskInfo.stTaskRtInfo.ulCount);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Cycletime (mcsec)</td>\n");
    printf("          <td>%d</td>\n", stMsgTaskInfo.stTaskRtInfo.ulCycleTime);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Cycletime min (mcsec)</td>\n");

    if(stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMin != (unsigned long)-1)
    {
      printf("           <td>%d</td>", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMin);
    }
    else
    {
      printf("           <td>-</td>");
    }

    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Cycletime max (mcsec)</td>\n");
    printf("          <td>%d</td>\n", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMax);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Cycletime avg (mcsec)</td>\n");

    if((stMsgTaskInfo.stTaskRtInfo.ulCount > 0) && (stMsgTaskInfo.stTaskRtInfo.ulCycleTimeAcc != -1))
    {
      printf("          <td>%d</td>\n", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeAcc/stMsgTaskInfo.stTaskRtInfo.ulCount);
    }
    else
    {
      printf("          <td>-</td>\n");
    }
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Status</td>\n");
    printf("          <td>%s</td>", snStatus);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Mode</td>\n");
    printf("          <td>%s</td>\n", snMode);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Priority</td>\n");
    printf("          <td>%d</td>\n", stMsgTaskInfo.stTaskDownloadFormat.ulPriority);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Interval (%s) </td>\n", snUnit);
    printf("          <td>%d</td>\n", interval);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Event</td>\n");
    if(stMsgTaskInfo.stTaskDownloadFormat.usEventRefid == (unsigned short)-1)
    {
      printf("          <td>NONE</td>\n");
    }
    else
    {
      printf("          <td>%d:%d:%d</td>\n", stMsgTaskInfo.stTaskDownloadFormat.usEventRefid, 
                                              stMsgTaskInfo.stTaskDownloadFormat.ulEventOffset,
                                              stMsgTaskInfo.stTaskDownloadFormat.usEventSize);
    }
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Function pointer</td>\n");
    printf("          <td>0x%08x</td>\n", stMsgTaskInfo.stTaskDescription.pfTask);
    printf("        </tr>\n");

    printf("        <tr>\n");
    printf("          <td>Function index</td>\n");
    printf("          <td>%d</td>\n", stMsgTaskInfo.stTaskDownloadFormat.ulPOUIndex);
    printf("        </tr>\n");

    ++taskNo;
    stMsgTaskInfo.taskNo = taskNo;
    status = ipcMsgCom_Send(ipcMsgCom_TASK_INFO, (void*)&stMsgTaskInfo, sizeof(stMsgTaskInfo), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);
  }

  return status;
}

int ShowTaskInfoListJson(void)
//
// Create the json object including all infos about the actual existing tasks in RTS.
// Get the data by message communication with PLC.
//
{
  int             status          = SUCCESS;
  int             taskNo          = 0;
  tstMsgTaskInfo  stMsgTaskInfo;

  stMsgTaskInfo.taskNo = 0;
  status = ipcMsgCom_Send(ipcMsgCom_TASK_INFO, (void*)&stMsgTaskInfo, sizeof(stMsgTaskInfo), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);

  if(status == SUCCESS)
  {
    printf("[ ");

    while((SUCCESS == status) && (stMsgTaskInfo.status == SUCCESS))
    {
      if(0 != taskNo) printf(", ");
      printf("{ ");

      char snStatus[MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO]  = "";
      char snMode[MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO]    = "";
      char snUnit[]                                         = "mcsec";

      long interval     = stMsgTaskInfo.stTaskDownloadFormat.lInterval;
      int  taskTimebase = stMsgTaskInfo.stTaskDownloadFormat.taskTimebase;

      GetTaskStatusString(stMsgTaskInfo.stTaskRtInfo.uStatus, snStatus);
      GetTaskModeString(stMsgTaskInfo.stTaskRtInfo.uMode, snMode);

      if((taskTimebase == CTRL_TASK_MILLIFACTOR) || ((taskTimebase == 1) && (interval % 1000 == 0)) )
      {
        strncpy(snUnit, "msec", sizeof(snUnit));
      }

      if((taskTimebase == CTRL_TASK_MILLIFACTOR) || ((taskTimebase == 1) && (interval % 1000 == 0)) )
      {
        interval = interval / 1000;
      }

      printf("\"taskName\":\"%s\", ", stMsgTaskInfo.stTaskDownloadFormat.cName);
      printf("\"taskId\":\"%d\", ", stMsgTaskInfo.stTaskDescription.ulTaskId);
      printf("\"cycleCount\":\"%d\", ", stMsgTaskInfo.stTaskRtInfo.ulCount);
      printf("\"cycleTime\":\"%d\", ", stMsgTaskInfo.stTaskRtInfo.ulCycleTime);

      if(stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMin != (unsigned long)-1)
      {
        printf("\"cycleTimeMin\":\"%d\", ", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMin);
      }
      else
      {
        printf("\"cycleTimeMin\":\"-\", ");
      }

      printf("\"cycleTimeMax\":\"%d\", ", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeMax);

      if((stMsgTaskInfo.stTaskRtInfo.ulCount > 0) && (stMsgTaskInfo.stTaskRtInfo.ulCycleTimeAcc != -1))
      {
        printf("\"cycleTimeAcc\":\"%d\", ", stMsgTaskInfo.stTaskRtInfo.ulCycleTimeAcc/stMsgTaskInfo.stTaskRtInfo.ulCount);
      }
      else
      {
        printf("\"cycleTimeAcc\":\"-\", ");
      }

      printf("\"status\":\"%s\", ", snStatus);
      printf("\"mode\":\"%s\", ", snMode);
      printf("\"priority\":\"%d\", ", stMsgTaskInfo.stTaskDownloadFormat.ulPriority);
      printf("\"snUnit\":\"%s\", ", snUnit);
      printf("\"interval\":\"%d\" ", interval);

      printf(" }");

      ++taskNo;
      stMsgTaskInfo.taskNo = taskNo;
      status = ipcMsgCom_Send(ipcMsgCom_TASK_INFO, (void*)&stMsgTaskInfo, sizeof(stMsgTaskInfo), ipcMsgCom_GET_RESPONSE, TIMEOUT_RECEIVE_ANSWER);
    }

    printf(" ]");
  }


  return status;
}

int ShowNrOfTasks(void)
{
  tstMsgTaskInfo  stMsgTaskInfo;
  stMsgTaskInfo.taskNo = 0;
  
  int status = SUCCESS;

  while (SUCCESS == (status = ipcMsgCom_Send(ipcMsgCom_TASK_INFO, 
                                             (void*)&stMsgTaskInfo, 
                                             sizeof(stMsgTaskInfo), 
                                             ipcMsgCom_GET_RESPONSE, 
                                             TIMEOUT_RECEIVE_ANSWER)) &&
         SUCCESS == stMsgTaskInfo.status)
  {
      stMsgTaskInfo.taskNo++;
  }

  printf("%d", stMsgTaskInfo.taskNo);

  return status;
}

int main(int    argc, 
         char** argv)
{
  int  status = SUCCESS;

  // help-text requested?
  if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)))
  {
    ShowHelpText();
  }

  else
  {if(argc < 2)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      char  outputString[MAX_LENGTH_OUTPUT_STRING_GET_RTS_INFO] = "";

      // plc state requested?
      if(strcmp(argv[1], "state") == 0)
      {
        status = GetPlcState(outputString);
        printf("%s", outputString);
      }

      // project info requested?
      else if(strcmp(argv[1], "project") == 0)
      {
        if(argc < 3)
        {
          status = MISSING_PARAMETER;
        }
        status = ShowProjectInfo(argv[2], outputString);
        printf("%s", outputString);
      }

      // task infos requested?
      else if(strcmp(argv[1], "task") == 0)
      {
        if(argc < 4)
        {
          status = MISSING_PARAMETER;
        }
        else
        {
          status = ShowTaskInfo(argv[2], argv[3], outputString);
          printf("%s", outputString);
        }
      }

      // html code for table with all task infos requested?
      else if(strcmp(argv[1], "task-list") == 0)
      {
        status = ShowTaskInfoList();
      }

      // html code for table with all task infos requested?
      else if(strcmp(argv[1], "task-list-json") == 0)
      {
        status = ShowTaskInfoListJson();
      }

      else if(strcmp(argv[1], "nr-of-tasks") == 0)
      {
        status = ShowNrOfTasks();
      }

      // input parameter are unknown
      else
      {
        status = INVALID_PARAMETER;
      }
    }
  }

  #if SHOW_ERRORS
  if(status != SUCCESS)
  {
    ShowErrorText(status, "");
    ShowHelpText();
  }
  #endif

  return(status);
}



        /*
        // Debug-Ausgabe Buffer:

        int byteNo = 0;
        char* bytePtr = (char*)pstRtsInfoShm;
        for(byteNo == 0; byteNo < sizeof(*pstRtsInfoShm); ++byteNo)
        {
          printf("%d:%c ", bytePtr[byteNo], bytePtr[byteNo]);
        }
        printf("\n");
        */
