//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
#ifndef _GET_RTS_INFO_H
#define _GET_RTS_INFO_H

// maximum count of CoDeSys tasks
#define MAX_TASK_COUNT                16

// maximum length of task download name
#define TASK_DOWNLOAD_NAME_LEN        32



// message structure to get plc state
typedef struct
{
  uint8_t state;
}
tstMsgPlcState;


// message structure to get sys lib project info
// like SYSLIBPROJECT_INFOtyp, RtsSysLib.h
typedef struct
{
  unsigned long DTDATE;
  char STPROJECT[256];
  char STTITLE[256];
  char STVERSION[256];
  char STAUTHOR[256];
  char STDESCRIPTION[1025];
}
tstMsgProjectInfo;


// part of task info structure
// like TASK_RTINFO, RtsTypes.h
typedef struct
{
  unsigned long   ulCount;
  unsigned long   ulCycleTime;
  unsigned long   ulCycleTimeMin;
  unsigned long   ulCycleTimeMax;
  unsigned long   ulCycleTimeAcc;
  unsigned short  uStatus;
  unsigned short  uMode;
}
tstTaskRtInfo;

// part of task info structure
// like TASK_DOWNLOAD_FORMAT, RtsTask.h
typedef struct 
{
  unsigned long   ulPriority;
  long            lInterval;
  unsigned short  usEventRefid;
  unsigned short  usEventSize;
  unsigned long   ulEventOffset;
  unsigned long   ulPOUIndex;
  char            cName[TASK_DOWNLOAD_NAME_LEN];
  int             taskTimebase;
}
tstTaskDownloadFormat;

// part of task info structure
// like TASK_DESCRIPTION, RtsTask.h
typedef struct
{
  unsigned long         ulTaskId;
  void*                 pfTask;
}
tstTaskDescription;


// structure of message content for getting task informations
typedef struct
{
  uint8_t               taskNo;
  int                   status;
  uint8_t               numberOfTasks;
  tstTaskRtInfo         stTaskRtInfo;
  tstTaskDownloadFormat stTaskDownloadFormat;
  tstTaskDescription    stTaskDescription;
}
tstMsgTaskInfo;


#endif
