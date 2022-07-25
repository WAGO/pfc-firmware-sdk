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
///  \file     $File:     $:
///
///  \version  $Revision: 65689 $:
///
///  \brief    <Part of the config-tools package: read the start/stop switch value>
///
///  \author   <AGa> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include <wago_oms_API.h>

#include "config_tool_lib.h"

#define OMS_INPUT_DEVICE "/dev/input/oms_switch"
#define OMS_INPUT_DEVICE_FALLBACK "/dev/input/event0"

#define RUN_STRING "run"
#define STOP_STRING "stop"

int main(int argc, char **argv)
{
  UNUSED_PARAMETER(argc);
  UNUSED_PARAMETER(argv);

  int ret = SUCCESS;
  int switchState = -1;
  
  tOmsDevice *omsDev = oms_OpenDevice(OMS_INPUT_DEVICE, OMS_MODE_PASSIVE);
  
  if(omsDev == NULL)
    omsDev = oms_OpenDevice(OMS_INPUT_DEVICE_FALLBACK, OMS_MODE_PASSIVE);

  if(omsDev != NULL)
  {
    if(OMS_RETURN_OK == omsDev->GetRunKey(omsDev, &switchState))
    {
      switch(switchState)
      {
      case 0:
        printf(STOP_STRING);
        break;
      case 1:
        printf(RUN_STRING);
        break;
      default:
        ret = SYSTEM_CALL_ERROR;
        break;
      }

      oms_CloseDevice(omsDev);
    } 
  }
  else
  {
    ret = FILE_OPEN_ERROR;
  }


  return ret;
}

