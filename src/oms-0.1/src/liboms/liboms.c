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
///  \file     liboms.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 1
#endif
#include "liboms_internal.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define OMS_DEVICE_VENDOR   0x0001
#define OMS_DEVICE_PRODUCT  0x0001
#define OMS_DEVICE_NAME     "PAC-Operating-Mode-Switch"

#define OMS_BOTTON_CLEAR(x) (x=0)

#define OMS_BIT_RESERVED 0x01
#define OMS_BIT_RUN      0x02
#define OMS_BIT_STOP     0x04
#define OMS_BIT_RESET    0x08
#define OMS_BIT_RSTALL   0x10

#define OMS_SETRUN_BOOL(x) ((x & OMS_BIT_RUN) >> 1)
#define OMS_SETSTOP_BOOL(x) ((x & OMS_BIT_STOP) >> 2)
#define OMS_SETRESET_BOOL(x) ((x & OMS_BIT_RESET) >> 3)
#define OMS_SETRSTALL_BOOL(x) ((x & OMS_BIT_RSTALL) >> 4)

#define OMS_CODE_RUN      1
#define OMS_CODE_STOP     2
#define OMS_CODE_RESET    3
#define OMS_CODE_RSTALL   4

#define OMS_BIT_IS_SET(x,y) (!!((x) & (y)))

struct timeval eventTime = {
                            .tv_sec =  0,
                            .tv_usec = 0,
};


/*struct _stOmsDevice{
    int fd;
    tOmsState bottons;
};*/

tOmsReturn   oms_GetRunKey           (tOmsDevice *, int *);
tOmsReturn   oms_GetStopKey          (tOmsDevice *, int *);
tOmsReturn   oms_GetResetKey         (tOmsDevice *, int *);
tOmsReturn   oms_GetRstAllKey        (tOmsDevice *, int *);
tOmsReturn   oms_GetRunKey_passive   (tOmsDevice *, int *);
tOmsReturn   oms_GetStopKey_passive  (tOmsDevice *, int *);
tOmsReturn   oms_GetResetKey_passive (tOmsDevice *, int *);
tOmsReturn   oms_GetRstAllKey_passive(tOmsDevice *, int *);
void         _FreeOmsDevice          (tOmsDevice *);
tOmsDevice * _CreateOmsStruct        (int);
tOmsReturn   _Verify_Device          (int);
tOmsReturn   _GetKey_passive(tOmsDevice *,
                             int,
                             tOmsReturn(*)(tOmsDevice *, int *),
                             int *);

tOmsDevice * _CreateOmsStruct(int fd)
{
  tOmsDevice *new =  malloc(sizeof(tOmsDevice));

  if(new != NULL)
  {
    new->fd = fd;
    OMS_BOTTON_CLEAR(new->bottons);
    new->watchLoopRun = 0;
  }

  return new;
}

void _FreeOmsDevice(tOmsDevice * dev)
{
  free(dev);
}

tOmsReturn oms_SetWatchMode(tOmsDevice * dev, tOmsMode mode)
{
  switch(mode)
  {
    case OMS_MODE_ACTIVE:
      dev->GetRunKey = oms_GetRunKey;
      dev->GetStopKey = oms_GetStopKey;
      dev->GetResetKey = oms_GetResetKey;
      dev->GetRstAllKey = oms_GetRstAllKey;
      dev->mode = OMS_MODE_ACTIVE;
      break;
    case OMS_MODE_PASSIVE:
    default:
      dev->GetRunKey = oms_GetRunKey_passive;
      dev->GetStopKey = oms_GetStopKey_passive;
      dev->GetResetKey = oms_GetResetKey_passive;
      dev->GetRstAllKey = oms_GetRstAllKey_passive;
      dev->mode = OMS_MODE_PASSIVE;
      break;
  }
  return OMS_RETURN_OK;
}

tOmsReturn _Verify_Device(int fd)
{
  tOmsReturn ret = OMS_RETURN_OK;
  struct input_id device_info;
  char name[256]= "Unknown";
  //int repeat[2];

  errno = 0;

  if(ioctl(fd, EVIOCGID, &device_info)) {
      perror("evdev ioctl");
      ret = OMS_RETURN_ERROR_IOCTL;
      goto out;
  }
  else
  {
    /*printf("vendor: %04hx\nproduct: %04hx\nversion: %04hx\nbustype: %04hx\n",
           device_info.vendor, device_info.product,
           device_info.version, device_info.bustype);*/
    if(   (device_info.vendor  != OMS_DEVICE_VENDOR)
       || (device_info.product != OMS_DEVICE_PRODUCT))
    {
      errno = ENODEV;
      ret = OMS_RETURN_ERROR_DEV_INFO;
      goto out;
    }
  }

  if(ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
      perror("evdev ioctl");
      ret = OMS_RETURN_ERROR_IOCTL;
      goto out;
  }
  else
  {
    if(strcmp(name, OMS_DEVICE_NAME))
    {
      errno = ENOKEY;
      ret = OMS_RETURN_ERROR_DEV_NAME;
      goto out;
    }
  }


  //#define EVIOCGREP   _IOR('E', 0x03, int[2])     /* get repeat settings */
  /*if(ioctl(fd, EVIOCGREP, repeat) < 0) {
      perror("event ioctl");
  }
  else
  {
    printf("The Device' Autorepeat is %d %d\n", repeat[0], repeat[1]);
  }*/
  /*repeat[0] = 5000;
  repeat[1] = 6000;

  if(ioctl(fd, EVIOCSREP, repeat) < 0) {
      perror("event ioctl");
  }
   */

out:
  return ret;
}

tOmsDevice * oms_OpenDevice(const char * path, tOmsMode mode)
{
  tOmsDevice *dev = NULL;
  int fd;

  fd = open(path,O_RDONLY);
  if(fd >= 0)
  {
    if(OMS_RETURN_OK == _Verify_Device(fd))
    {
      dev = _CreateOmsStruct(fd);
      oms_SetWatchMode(dev, mode);
    }
  }

  return dev;
}

void oms_CloseDevice(tOmsDevice * dev)
{
  close(dev->fd);
  _FreeOmsDevice(dev);
}

tOmsReturn _GetKey_passive(tOmsDevice *dev,
                           int iocmd,
                           tOmsReturn(*GetFunc)(tOmsDevice *, int *),
                           int * ret)
{
  if(-1 != ioctl(dev->fd, iocmd, &dev->bottons))
  {
    return GetFunc(dev,ret);
  }
  else
  {
    return OMS_RETURN_ERROR;
  }
}

tOmsReturn oms_GetRunKey(tOmsDevice *dev, int * result)
{
  *result = OMS_SETRUN_BOOL(dev->bottons);
  return OMS_RETURN_OK;
}

tOmsReturn oms_GetStopKey(tOmsDevice *dev, int * result)
{
  *result = OMS_SETSTOP_BOOL(dev->bottons);
  return OMS_RETURN_OK;
}

tOmsReturn oms_GetResetKey(tOmsDevice *dev, int * result)
{
  *result = OMS_SETRESET_BOOL(dev->bottons);
  return OMS_RETURN_OK;
}

tOmsReturn oms_GetRstAllKey(tOmsDevice *dev, int * result)
{
  *result = OMS_SETRSTALL_BOOL(dev->bottons);
  return OMS_RETURN_OK;
}

tOmsReturn oms_GetRunKey_passive(tOmsDevice *dev, int * result)
{
  return _GetKey_passive(dev, EVIOCGSW(sizeof(tOmsState)), oms_GetRunKey, result);
}

tOmsReturn oms_GetStopKey_passive(tOmsDevice *dev, int * result)
{
  return _GetKey_passive(dev, EVIOCGSW(sizeof(tOmsState)), oms_GetStopKey, result);
}

tOmsReturn oms_GetResetKey_passive(tOmsDevice *dev, int * result)
{
  return _GetKey_passive(dev, EVIOCGKEY(sizeof(tOmsState)), oms_GetResetKey, result);
}

tOmsReturn oms_GetRstAllKey_passive(tOmsDevice *dev, int * result)
{
  return _GetKey_passive(dev, EVIOCGKEY(sizeof(tOmsState)), oms_GetRstAllKey, result);
}

void _SetKeyBit(uint16_t code, bool set, tOmsState *buttons)
{
  tOmsState mask;
  mask = 1 << code;
  if(set == true)
  {
    *buttons |= mask;
  }
  else
  {
    *buttons &= ~mask;
  }
}

void _UpdateKeysAndSwitches(tOmsDevice * dev)
{
  int bytes;
  struct input_event event;

  bytes = read(dev->fd, &event, sizeof(struct input_event));
  if(bytes == sizeof(struct input_event))
  {
    //set key
    if(   (event.type == EV_KEY)
       || (event.type == EV_SW))
    {
      _SetKeyBit(event.code, (bool) event.value, &dev->bottons);
      eventTime.tv_sec  = event.time.tv_sec;
      eventTime.tv_usec = event.time.tv_usec;

    }
  }
}

tOmsReturn   oms_Watch(tOmsDevice * dev)
{
  struct pollfd fds;
  int    timeout;
  int pollret;
  tOmsReturn ret = OMS_RETURN_OK;
  tOmsState switches;
  tOmsState keys;

  if(dev->mode != OMS_MODE_ACTIVE)
  {
    return OMS_RETURN_ERROR;
  }

  if(-1 == ioctl(dev->fd, EVIOCGSW(sizeof(tOmsState)), &switches))
  {
    return OMS_RETURN_ERROR;
  }
  if(-1 == ioctl(dev->fd, EVIOCGKEY(sizeof(tOmsState)), &keys))
  {
    return OMS_RETURN_ERROR;
  }
  dev->bottons = (keys | switches);

  fds.fd = dev->fd;
  fds.events = POLLIN | POLLPRI | POLLRDBAND;
  fds.revents = 0;
  dev->watchLoopRun = 1;
  timeout = -1;
  //first check states one time and than go into poll
  pollret = 0;
  while(dev->watchLoopRun)
  {
    if(pollret > 0)
    {
      //something happend at the input device
      _UpdateKeysAndSwitches(dev);
      timeout = SM_UpdateStateMaschine(dev);
    }
    else if(pollret == 0)
    {
      //timeout occured
      gettimeofday(&eventTime, NULL);
      timeout = SM_UpdateStateMaschine(dev);
    }
    else
    {
      //error
      dev->watchLoopRun = 0;
      ret = OMS_RETURN_ERROR;
    }
    pollret = poll(&fds, 1, timeout);
  }

  return ret;
}

tOmsReturn   oms_StopWatch(tOmsDevice * dev)
{
  dev->watchLoopRun = 0;
  return OMS_RETURN_OK;
}

//---- End of source file ------------------------------------------------------
