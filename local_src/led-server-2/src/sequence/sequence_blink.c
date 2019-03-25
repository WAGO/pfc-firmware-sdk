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
///  \file     led_blink_periodic.c
///
///  \version  $Revision: 12545 $
///
///  \brief    This File serves all functions for periodical blinking
///
///  \author   Hans Florian Scholz : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include "leds.h"
//#include "LedCtl_API.h"
#include "led_schedule.h"
#include "led_multi.h"
//#include "led_blink_periodic.h"
#include "led_blink_sequential.h"
#include <ledserver_API.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
//#define MAX_CHANGES 2         /* max changes of LED-states */

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

int LEDHANDLER_SetBlink(tLedNr ledNr, void * data, void * user_data)
{
  (void)user_data;
  tLedBlink * blink = (tLedBlink*) data;
  tMultiSequence  sequence;
  tMultiPeriode * period = NULL;
  tMultiElement * element = NULL;
  if(blink == NULL)
  {
    return -1;
  }
  MULTI_InitSequence(&sequence);
  period = MULTI_NewPeriod(1,LED_OFF,blink->time1+blink->time2);
  element = MULTI_NewElement(blink->color1, blink->time1);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(blink->color2, blink->time2+blink->time1);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);
  MULTI_SetSequenceEnd(&sequence,MULTI_SEQUENCE_RESTART);
  if(LED_RETURN_OK == SEQUENTIAL_SetBlinkSequence(ledNr,&sequence))
  {
    ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLedBlink));
    return 0;
  }
  else
  {
    MULTI_DeinitSequence(&sequence);
    return -1;
  }
}

//---- End of source file ------------------------------------------------------
