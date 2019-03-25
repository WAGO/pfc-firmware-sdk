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
///  \file     LedBlinkSequential.c
///
///  \version  $Revision: 18164 $
///
///  \brief    This File serves all functions for sequential blinking
///
///  \author   Hans Florian Scholz: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "config.h"
#include "common.h"
#include "leds.h"
#include "led_schedule.h"
#include "led_blink_sequential.h"
#include "led_multi.h"
#include <ledserver_API.h>

void FreeMulti(tMultiPeriode * pMulti);

//-- Function: LedBlinkSequential ------------------------------------------------
///
///  Realise the sequential Blink Function one change per call
///
///  \param ledNo Nummer der zu bearbeitenden LED
///  \param aktTime Aktueller Zeitpunkt ind Sekunden und Nanosekunden
///
//------------------------------------------------------------------------------
/*void SEQUENTIAL_LedBlinkSequential(uint16_t         ledNo,
                                   struct timespec  *aktTime)*/
void SEQUENTIAL_LedBlinkSequential(uint16_t ledNo,
                                   tTimeMS  *aktTime)
{
  tTimeMS nextTime;
  tTimeMS timeBase;

    if(SCHEDULE_led[ledNo].blinkcounter == 0)
    {
      if(SCHEDULE_led[ledNo].pMultiAct->pNext != NULL)
      {
        SCHEDULE_led[ledNo].pMultiAct = SCHEDULE_led[ledNo].pMultiAct->pNext;
        SCHEDULE_led[ledNo].pMultiAct->pAct = SCHEDULE_led[ledNo].pMultiAct->list;
        SCHEDULE_led[ledNo].blinkcounter = SCHEDULE_led[ledNo].pMultiAct->periods;
        if(SCHEDULE_led[ledNo].blinkcounter != 0)
        {
          SEQUENTIAL_LedBlinkSequential(ledNo, aktTime);
        }
      }
      else
      {
        SCHEDULE_led[ledNo].state = LED_STATE_STATIC;
        ledserver_LEDCTRL_SetLedStateData(ledNo,(void*)&(SCHEDULE_led[ledNo].pMultiAct->staticColor),sizeof(tLedStatic));

        SetLed(ledNo,(uint8_t)SCHEDULE_led[ledNo].pMultiAct->staticColor);
        SCHEDULE_led[ledNo].active = 0;
        nextTime = END_OF_MSECONDS_64 - *aktTime;
        timeBase = 1;
        MULTI_FreeMulti(SCHEDULE_led[ledNo].pMulti);
        SCHEDULE_led[ledNo].pMulti    = NULL;
        SCHEDULE_led[ledNo].pMultiAct = NULL;
      }

    }
    else
    {
      SetLed(ledNo,(uint8_t)SCHEDULE_led[ledNo].pMultiAct->pAct->color);
      nextTime = SCHEDULE_led[ledNo].pMultiAct->pAct->change;
      timeBase = SCHEDULE_led[ledNo].pMultiAct->timeBase;
      if(SCHEDULE_led[ledNo].pMultiAct->pAct->pNext == NULL)
      {
        SCHEDULE_led[ledNo].blinkcounter--;
        SCHEDULE_led[ledNo].pMultiAct->pAct = SCHEDULE_led[ledNo].pMultiAct->list;
      }
      else
      {
        SCHEDULE_led[ledNo].pMultiAct->pAct = SCHEDULE_led[ledNo].pMultiAct->pAct->pNext;
      }
      if(timeBase == 0)
      {
        timeBase = 1;
      }
      SCHEDULE_SetNextChangeTime( &(SCHEDULE_led[ledNo].nextChange),
                                   (const tTimeMS)  nextTime,
                                   (const tTimeMS)  timeBase,
                                   (const tTimeMS*)  aktTime);
    }
}

tLedReturnCode SEQUENTIAL_SetBlinkSequence(tLedNr ledNr, tMultiSequence * sequence)
{
   if(ledNr >= GetNoOfLeds())
   {
     return LED_RETURN_ERROR_NUMBER;
   }
   if(sequence == NULL)
   {
     return LED_RETURN_ERROR_PARAMETER;
   }

   pthread_mutex_lock(&SCHEDULE_led[ledNr].mutexBlinkSeq);
   if(sequence->periods != NULL)
   {
     SCHEDULE_led[ledNr].active = 1;
     SCHEDULE_led[ledNr].pMulti = sequence->periods;
     SCHEDULE_led[ledNr].pMultiAct = SCHEDULE_led[ledNr].pMulti;
     SCHEDULE_led[ledNr].blinkcounter = SCHEDULE_led[ledNr].pMulti->periods;
   }
   pthread_mutex_unlock(&SCHEDULE_led[ledNr].mutexBlinkSeq);
   return LED_RETURN_OK;
}

//---- End of source file ------------------------------------------------------
