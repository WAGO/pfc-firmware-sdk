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
///  \file     sequence_750.c
///
///  \version  $Rev: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "led_multi.h"
#include "led_blink_sequential.h"

#ifdef PFC200_ADV
#define BLINK_SEQ_LED_MAGENTA LED_COLOR_MAGENTA
#define BLINK_SEQ_LED_RED LED_COLOR_RED
#define BLINK_SEQ_LED_BLUE LED_COLOR_BLUE
#else
#define BLINK_SEQ_LED_MAGENTA LED_COLOR_RED
#define BLINK_SEQ_LED_RED LED_COLOR_RED
#define BLINK_SEQ_LED_BLUE LED_COLOR_RED
#endif

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
  element = MULTI_NewElement(BLINK_SEQ_LED_MAGENTA, BLINK_SEQEUNZ_750_PEAMBLE_MSEC1);
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
  element = MULTI_NewElement(BLINK_SEQ_LED_RED, BLINK_SEQEUNZ_750_SEQ1_MSEC1);
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
  element = MULTI_NewElement(BLINK_SEQ_LED_BLUE, BLINK_SEQEUNZ_750_SEQ2_MSEC1);
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
