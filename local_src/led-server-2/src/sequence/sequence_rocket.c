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


#define BLINK_SEQEUNZ_ROCKET_TIME_1HZ                 (1000/2)
#define BLINK_SEQEUNZ_ROCKET_TIME_10HZ                (1000/20)
#define BLINK_SEQEUNZ_ROCKET_TIME_20HZ                (1000/50)


#define BLINK_SEQEUNZ_ROCKET_SYNC_COUNT               10
#define BLINK_SEQEUNZ_ROCKET_SYNC_PERIODFACTOR        2
#define BLINK_SEQEUNZ_ROCKET_SYNC_TIME                BLINK_SEQEUNZ_ROCKET_TIME_10HZ

#define BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT              1
#define BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR       1
#define BLINK_SEQEUNZ_ROCKET_PAUSE_TIME               1000

#define BLINK_SEQEUNZ_ROCKET_END_COUNT                1
#define BLINK_SEQEUNZ_ROCKET_END_PERIODFACTOR         1
#define BLINK_SEQEUNZ_ROCKET_END_TIME                 4000

#define BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR         2
#define BLINK_SEQEUNZ_ROCKET_ERR_TIME                 BLINK_SEQEUNZ_ROCKET_TIME_1HZ

#define BLINK_SEQEUNZ_ROCKET_ZERO_COUNT               10
#define BLINK_SEQEUNZ_ROCKET_ZERO_PERIODFACTOR        2
#define BLINK_SEQEUNZ_ROCKET_ZERO_TIME                BLINK_SEQEUNZ_ROCKET_TIME_20HZ

#define BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT          1
#define BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_PERIODFACTOR   1
#define BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME           1500

#define BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT          1
#define BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_PERIODFACTOR   1
#define BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME           40

int LEDHANDLER_SetRocketErr2(tLedNr ledNr, void * data, void * user_data)
{
  (void)user_data;
  tLedRocketErr2 * seq = (tLedRocketErr2*) data;
  tMultiSequence  sequence;
  tMultiPeriode * period = NULL;
  tMultiElement * element = NULL;

  if(seq == NULL)
  {
    return -1;
  }

  MULTI_InitSequence(&sequence);

  //we start with the last pause because of switch between sequences
  //PAUSE 4s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_END_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_END_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_END_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_END_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //SYNCBLINK 10x red 10Hz 50%
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_SYNC_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_SYNC_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //ERRORGROUP x-times red 1Hz 50%
  period = MULTI_NewPeriod(seq->errorGroup, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //SYNCBLINK 10x magenta 10Hz 50%
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_SYNC_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_SYNC_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  element = MULTI_NewElement(LED_COLOR_MAGENTA, BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //ERRORCODE x-times magenta 1Hz 50%
  period = MULTI_NewPeriod(seq->errorCode, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  element = MULTI_NewElement(LED_COLOR_MAGENTA, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  MULTI_SetSequenceEnd(&sequence,MULTI_SEQUENCE_RESTART);

  if(LED_RETURN_OK == SEQUENTIAL_SetBlinkSequence(ledNr,&sequence))
  {
    ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLedRocketErr2));
    return 0;
  }
  else
  {
    MULTI_DeinitSequence(&sequence);
    return -1;
  }
}


int LEDHANDLER_SetRocketErr3(tLedNr ledNr, void * data, void * user_data)
{
  (void)user_data;
  tLedRocketErr3 * seq = (tLedRocketErr3*) data;
  tMultiSequence  sequence;
  tMultiPeriode * period = NULL;
  tMultiElement * element = NULL;

  uint8_t address = seq->errorAddress;

  uint8_t one = (address) % 10;
  uint8_t ten = (((address) - one) % 100) / 10;
  uint8_t hundred = ((address) - one - (ten * 10)) / 100;

  if(seq == NULL)
  {
    return -1;
  }

  MULTI_InitSequence(&sequence);

  //we start with the last pause because of switch between sequences
  //PAUSE 4s
//  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_END_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_END_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_END_TIME);
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_END_COUNT, LED_COLOR_OFF, 1);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_END_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //SYNCBLINK 10x red 10Hz 50%
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_SYNC_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_SYNC_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //ERRORGROUP x-times red 1Hz 50%
  period = MULTI_NewPeriod(seq->errorGroup, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  element = MULTI_NewElement(LED_COLOR_RED, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //SYNCBLINK 10x magenta 10Hz 50%
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_SYNC_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_SYNC_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  element = MULTI_NewElement(LED_COLOR_MAGENTA, BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //ERRORCODE x-times magenta 1Hz 50%
  period = MULTI_NewPeriod(seq->errorCode, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  element = MULTI_NewElement(LED_COLOR_MAGENTA, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //SYNCBLINK 10x blue 10Hz 50%
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_SYNC_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_SYNC_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_SYNC_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  //PAUSE 1s
  period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
  MULTI_AddElementToPeriod(period,element);
  MULTI_AddPeriodToSequence(&sequence,period);

  if(0 != hundred)
  {
    //ADDRESS HUNDRED x-times blue 1Hz 50%
//    period = MULTI_NewPeriod(hundred, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
    period = MULTI_NewPeriod(hundred, LED_COLOR_OFF, 1);
    element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
    MULTI_AddElementToPeriod(period,element);
    element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
    MULTI_AddElementToPeriod(period,element);
    MULTI_AddPeriodToSequence(&sequence,period);

    //PAUSE 1s
//    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, 1);
    element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
    MULTI_AddElementToPeriod(period,element);
    MULTI_AddPeriodToSequence(&sequence,period);

    //ADDRESSDELIM 1x flash 40ms blue
//    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME);
    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT, LED_COLOR_OFF, 1);
    element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME);
    MULTI_AddElementToPeriod(period,element);
    MULTI_AddPeriodToSequence(&sequence,period);

    //PAUSE 1,5s
//    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, 1);
    element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
    MULTI_AddElementToPeriod(period,element);
    MULTI_AddPeriodToSequence(&sequence,period);
  }
  else
  {
    //Do not display leading zero
  }

  if(10 <= address)
  {
    if(0 != ten)
    {
      //ADDRESS TEN x-times blue 1Hz 50%
//      period = MULTI_NewPeriod(ten, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
      period = MULTI_NewPeriod(ten, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
      MULTI_AddElementToPeriod(period,element);
      element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);

      //PAUSE 1s
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_PAUSE_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_PAUSE_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);

      //ADDRESSDELIM 1x flash 40ms blue
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);

      //PAUSE 1,5s
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);
    }
    else
    {
      //ADDRESS TEN==0 10x blue 20Hz 50%
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ZERO_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ZERO_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ZERO_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ZERO_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ZERO_TIME);
      MULTI_AddElementToPeriod(period,element);
      element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ZERO_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);

      //PAUSE 1,5s
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);

      //ADDRESSDELIM 1x flash 40ms blue
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ADDRESSDELIM_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);

      //PAUSE 1,5s
//      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
      period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_COUNT, LED_COLOR_OFF, 1);
      element = MULTI_NewElement(LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ADDRESSPAUSE_TIME);
      MULTI_AddElementToPeriod(period,element);
      MULTI_AddPeriodToSequence(&sequence,period);
    }
  }
  else
  {
    //Do not display leading zero
  }

  if(0 != one)
  {
    //ADDRESS ONE x-times blue 1Hz 50%
//    period = MULTI_NewPeriod(one, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ERR_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
    period = MULTI_NewPeriod(one, LED_COLOR_OFF, 1);
    element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ERR_TIME);
    MULTI_AddElementToPeriod(period,element);
    element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ERR_TIME);
    MULTI_AddElementToPeriod(period,element);
    MULTI_AddPeriodToSequence(&sequence,period);
  }
  else
  {
    //ADDRESS ONE==0 10x blue 20Hz 50%
//    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ZERO_COUNT, LED_COLOR_OFF, BLINK_SEQEUNZ_ROCKET_ZERO_PERIODFACTOR * BLINK_SEQEUNZ_ROCKET_ZERO_TIME);
    period = MULTI_NewPeriod(BLINK_SEQEUNZ_ROCKET_ZERO_COUNT, LED_COLOR_OFF, 1);
    element = MULTI_NewElement(LED_COLOR_BLUE, BLINK_SEQEUNZ_ROCKET_ZERO_TIME);
    MULTI_AddElementToPeriod(period,element);
    element = MULTI_NewElement(LED_COLOR_OFF, 2 * BLINK_SEQEUNZ_ROCKET_ZERO_TIME);
    MULTI_AddElementToPeriod(period,element);
    MULTI_AddPeriodToSequence(&sequence,period);
  }

  MULTI_SetSequenceEnd(&sequence,MULTI_SEQUENCE_RESTART);

  if(LED_RETURN_OK == SEQUENTIAL_SetBlinkSequence(ledNr,&sequence))
  {
    ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLedRocketErr2));
    return 0;
  }
  else
  {
    MULTI_DeinitSequence(&sequence);
    return -1;
  }
}
//---- End of source file ------------------------------------------------------
