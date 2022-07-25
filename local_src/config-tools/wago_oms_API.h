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
///  \file     liboms.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LIBOMS_H_
#define LIBOMS_H_

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <sys/time.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define OMS_WATCH_MASK            0x01
#define OMS_WATCH_UP              0x02
#define OMS_WATCH_TIMEOUT         0x04
#define OMS_WATCH_NOT_TIMEOUT     0x08
#define OMS_WATCH_NEED_ACTIVE     0x10
#define OMS_WATCH_NOT_ACTIVE      0x20

#define OMS_MSTATE_ACTIVE         0x01
#define OMS_MSTATE_READY          0x02
#define OMS_MSTATE_ALLWAYS_ACTIVE 0x04
#define OMS_MSTATE_TIME_SET       0x08

#define OMS_WMASK_RUN      0x02
#define OMS_WMASK_STOP     0x04
#define OMS_WMASK_RESET    0x08
#define OMS_WMASK_RSTALL   0x10
//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef enum eOmsReturn {
  OMS_RETURN_OK = 0,
  OMS_RETURN_ERROR,
  OMS_RETURN_ERROR_IOCTL,
  OMS_RETURN_ERROR_DEV_INFO,
  OMS_RETURN_ERROR_DEV_NAME
}tOmsReturn;
typedef enum eOmsMode {
  OMS_MODE_PASSIVE,  /* ask manually for switch states */
  OMS_MODE_ACTIVE    /* server mode starts a new thread and safe only switch interupts */
} tOmsMode;

typedef unsigned char tOmsState;
typedef struct stOmsDevice
{
    int fd;
    tOmsState bottons;
    tOmsMode  mode;
    int       watchLoopRun;
    tOmsReturn (*GetRunKey)   (struct stOmsDevice *dev, int * result);
    tOmsReturn (*GetStopKey)  (struct stOmsDevice *dev, int * result);
    tOmsReturn (*GetResetKey) (struct stOmsDevice *dev, int * result);
    tOmsReturn (*GetRstAllKey)(struct stOmsDevice *dev, int * result);
}tOmsDevice;

typedef unsigned char tOmsWatchFlags;
typedef unsigned char tOmsMaschineStateFlags;

typedef void (*tOmsEventfunction)(void *args);

typedef struct stOmsEventFunctionCall
{
    void (*func)(void *args);
    void * args;
    struct stOmsEventFunctionCall *pNext;
}tOmsEventFunctionCall;

typedef struct stOmsStateSwitch tOmsStateSwitch;

typedef struct stOmsStateCallList {
    tOmsStateSwitch * sSwitch;
    struct stOmsStateCallList * pNext;
}tOmsStateCallList;

struct stOmsStateSwitch {
    tOmsState                 MaskWatch;
    tOmsState                 MaskState;
    tOmsWatchFlags            flagsWatch;
    tOmsWatchFlags            flagsState;
    int                       timeoutMsec;
    struct timeval            timeOccured;
    tOmsMaschineStateFlags     maschineStateFlags;
    tOmsEventFunctionCall   * callFunc;
    tOmsStateCallList       * callStates;
    tOmsStateCallList       * callNotActive;
    tOmsStateCallList       * callNeedActive;
    struct stOmsStateSwitch * pNext;
};


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------



//-- Function:  oms_OpenDevice -------------------------------------------------
///
///  Opens an Inputdevice given by path and set up it for the given watchmode.
///  Watchmode shoult be:
///  OMS_MODE_ACTIVE: The Watch() method can be used to watch for switch events
///  OMS_MODE_PASSIVE: only use GetKey functions to get the states of the keys and switches
///  Sets errno:
///  EBADF  not a valid descriptor.
///  EFAULT inaccessible memory area.
///  EINVAL Request or argp is not valid.
///  ENOTTY d is not associated with a character special device.
///  ENOTTY The specified request does not apply to the kind of object that the descriptor d references.
///  ENODEV invalid device-info
///  ENOKEY invalid devicekey
///  EACCES Search permission is denied on a component of the path prefix, or
///         the file exists and  the  permissions  specified by oflag are denied,
///         or the file does not exist and write permission is denied for the
///         parent directory of the file to be created, or O_TRUNC is specified
///         and  write  permission  is denied.
///  EINTR  A signal was caught during open().
///  EINVAL The implementation does not support synchronized I/O for this file.
///  EIO    The path argument names a STREAMS file and a hangup or error occurred during the open().
///  ELOOP  A loop exists in symbolic links encountered during resolution of the path argument.
///  EMFILE {OPEN_MAX} file descriptors are currently open in the calling process.
///  ENAMETOOLONG The  length  of  the  path  argument  exceeds  {PATH_MAX}  or
///               a  pathname component is longer than {NAME_MAX}.
///  ENFILE The maximum allowable number of files is currently open in the system.
///  ENOENT the path does not exist
///  ENOSR  The path argument names a STREAMS-based file and the system is unable to allocate a STREAM.
///  ENXIO  The  named  file  is a character special or block special file, and the device associated with this
///         special file does not exist.
///  EOVERFLOW The named file is a regular file and the size of the file cannot
///            be  represented  correctly  in  an object of type off_t.
///  EAGAIN The path argument names the slave side of a pseudo-terminal device that is locked.
///
///
///  \param  path path to the input device (normally "/dev/input/event0")
///  \param  mode watching mode: (OMS_MODE_ACTIVE, OMS_MODE_PASSIVE)
///
///  \return a pointer to a tOmsDevice or NULL on error
///
//------------------------------------------------------------------------------
tOmsDevice * oms_OpenDevice(const char * path, tOmsMode mode);

//-- Function:  oms_CloseDevice -----------------------------------------------
///
///  Closes A opened tOmsDevice
///
///  \param  dev  pointer to an instance of tOmsDevice
///
///  \return OMS_RETURN_OK or OMS_RETURN_ERROR
///
//------------------------------------------------------------------------------
void oms_CloseDevice(tOmsDevice * dev);

//-- Function:  oms_SetWatchMode -----------------------------------------------
///
///  Set/Change the watchmode of the given tOmsDevice.
///
///  \param  dev  pointer to an instance of tOmsDevice
///  \param  mode watching mode: (OMS_MODE_ACTIVE, OMS_MODE_PASSIVE)
///
///  \return OMS_RETURN_OK or OMS_RETURN_ERROR
///
//------------------------------------------------------------------------------
tOmsReturn   oms_SetWatchMode(tOmsDevice * dev, tOmsMode mode);

//-- Function:  oms_Watch ------------------------------------------------------
///
///  Watches the given tOmsDevice and call the previos setted routines in case
///  of the defined events.
///  This function Blocks until oms_StopWatch() was called;
///
///  \param  dev  pointer to an instance of tOmsDevice
///
///  \return OMS_RETURN_OK or OMS_RETURN_ERROR
///
//------------------------------------------------------------------------------
tOmsReturn   oms_Watch(tOmsDevice * dev);


//-- Function:  oms_Watch ------------------------------------------------------
///
///  Watches the given tOmsDevice and call the previos setted routines in case
///  of the defined events.
///  This function Blocks until oms_StopWatch() was called;
///
///  \param  dev  pointer to an instance of tOmsDevice
///
///  \return OMS_RETURN_OK or OMS_RETURN_ERROR
///
//------------------------------------------------------------------------------
tOmsReturn   oms_StopWatch(tOmsDevice * dev);

void oms_SM_PrintSwitches(void);
void oms_SM_CleanStateSwitch(tOmsStateSwitch * pSwitch);
tOmsStateSwitch * oms_SM_NewStateSwitch(int StartElement);
tOmsReturn oms_SM_SetWatchMask(tOmsStateSwitch * pSwitch,
                               tOmsState watch,
                               tOmsState values);
tOmsReturn oms_SM_SetTimeout(tOmsStateSwitch * pSwitch,
                             int timeout);
tOmsReturn oms_SM_SetOnButtomUp(tOmsStateSwitch * pSwitch,
                                int up);
tOmsReturn oms_SM_SetCallFunction(tOmsStateSwitch * pSwitch,
                                  void (*func)(void *),
                                  void * args);
tOmsReturn oms_SM_RegisterSwitch( tOmsStateSwitch *pSwitch);
tOmsReturn oms_SM_SetFollowSwitch(tOmsStateSwitch *pSwitch,
                                  tOmsStateSwitch *pFollow);
tOmsReturn oms_SM_SetNeedActiveSwitch(tOmsStateSwitch *pSwitch,
                                      tOmsStateSwitch *pActive);
tOmsReturn oms_SM_SetNotActiveSwitch(tOmsStateSwitch *pSwitch,
                                      tOmsStateSwitch *pNotActive);


#endif /* LIBOMS_H_ */
//---- End of source file ------------------------------------------------------
