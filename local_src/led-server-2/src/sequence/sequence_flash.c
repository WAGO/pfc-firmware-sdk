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
///  \file     sequence_flash.c
///
///  \version  $Rev: 12545 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "led_multi.h"
#include "led_blink_sequential.h"

int LEDHANDLER_SetFlash(tLedNr ledNr, void * data, void * user_data)
{
  (void) user_data;
  tLedFlash * flash = (tLedFlash*) data;
  tMultiSequence  sequence;
  tMultiPeriode * period = NULL;
  tMultiElement * element = NULL;
  if(flash == NULL)
  {
    return -1;
  }
  MULTI_InitSequence(&sequence);
  period = MULTI_NewPeriod(1,flash->staticColor,1);
  element = MULTI_NewElement(flash->flashColor, flash->flashTime);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);
  MULTI_SetSequenceEnd(&sequence,MULTI_SEQUENCE_STOP);
  if(LED_RETURN_OK == SEQUENTIAL_SetBlinkSequence(ledNr,&sequence))
  {
    ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLedFlash));
    return 0;
  }
  else
  {
    MULTI_DeinitSequence(&sequence);
    return -1;
  }
}

//---- End of source file ------------------------------------------------------
