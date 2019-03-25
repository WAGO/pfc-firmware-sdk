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
///  \file     sequence_750.c
///
///  \version  $Rev: 12545 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//#include "sequence_750.h"
#include "led_multi.h"
#include "led_blink_sequential.h"

#define BLINK_SEQEUNZ_750_PEAMBLE            10
#define BLINK_SEQEUNZ_750_PEAMBLE_MSEC       50
#define BLINK_SEQEUNZ_750_PEAMBLE_MSEC1      50
#define BLINK_SEQEUNZ_750_PEAMBLE_MSEC2      100
#define BLINK_SEQEUNZ_750_PEAMBLE_TBASE      100

#define BLINK_SEQEUNZ_750_SEQ1_MSEC          500
#define BLINK_SEQEUNZ_750_SEQ1_MSEC1         500
#define BLINK_SEQEUNZ_750_SEQ1_MSEC2         1000
#define BLINK_SEQEUNZ_750_SEQ1_TBASE         1000

#define BLINK_SEQEUNZ_750_SEQ2_MSEC          500
#define BLINK_SEQEUNZ_750_SEQ2_MSEC1         500
#define BLINK_SEQEUNZ_750_SEQ2_MSEC2         1000
#define BLINK_SEQEUNZ_750_SEQ2_TBASE         1000

#define BLINK_SEQEUNZ_750_PAUSE_MSEC         1000
#define BLINK_SEQEUNZ_750_PAUSE_PRE_TBASE    BLINK_SEQEUNZ_750_PEAMBLE_TBASE
#define BLINK_SEQEUNZ_750_PAUSE_SEQ1_TBASE   BLINK_SEQEUNZ_750_SEQ1_TBASE
#define BLINK_SEQEUNZ_750_PAUSE_SEQ2_TBASE   BLINK_SEQEUNZ_750_SEQ2_TBASE

int LEDHANDLER_SetErr750(tLedNr ledNr, void * data, void * user_data)
{
  (void)user_data;
  tLed750 * seq = (tLed750*) data;
  tMultiSequence  sequence;
  tMultiPeriode * period = NULL;
  tMultiElement * element = NULL;
  if(seq == NULL)
  {
    return -1;
  }
  MULTI_InitSequence(&sequence);

  //we start with the last pause because of switch between sequences
  //PAUSE 3
  period = MULTI_NewPeriod(1,LED_COLOR_OFF,
                                      BLINK_SEQEUNZ_750_PAUSE_SEQ2_TBASE);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_750_PAUSE_MSEC);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);


  //PREAMBLE
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_750_PEAMBLE,
                           LED_COLOR_OFF,
                           BLINK_SEQEUNZ_750_PEAMBLE_TBASE);;
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_750_PEAMBLE_MSEC1);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_750_PEAMBLE_MSEC2);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1
  period = MULTI_NewPeriod(1,LED_COLOR_OFF,
                           BLINK_SEQEUNZ_750_PAUSE_PRE_TBASE);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_750_PAUSE_MSEC);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //ERRORCODE
  period = MULTI_NewPeriod(seq->errorCode, LED_COLOR_OFF, BLINK_SEQEUNZ_750_SEQ1_TBASE);
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_750_SEQ1_MSEC1);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_750_SEQ1_MSEC2);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 2
  period = MULTI_NewPeriod(1,LED_COLOR_OFF,
                                      BLINK_SEQEUNZ_750_PAUSE_SEQ1_TBASE);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_750_PAUSE_MSEC);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //ERRORARG
  period = MULTI_NewPeriod(seq->errorArg, LED_COLOR_OFF, BLINK_SEQEUNZ_750_SEQ2_TBASE);
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_750_SEQ2_MSEC1);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_750_SEQ2_MSEC2);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  MULTI_SetSequenceEnd(&sequence,MULTI_SEQUENCE_RESTART);

  if(LED_RETURN_OK == SEQUENTIAL_SetBlinkSequence(ledNr,&sequence))
  {
    ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLed750));
    return 0;
  }
  else
  {
    MULTI_DeinitSequence(&sequence);
    return -1;
  }
}
//---- End of source file ------------------------------------------------------
