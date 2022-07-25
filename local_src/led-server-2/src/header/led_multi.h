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
///  \file     seq_special.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LED_MULTI_H_
#define LED_MULTI_H_

#include "led_schedule.h"

typedef struct stMultiParams{
    uint8_t  state;
    uint16_t var1;
    uint16_t var2;
}tMultiParams;

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

#define BLINK_SEQUENZ_SPECIAL_CAN_PREOP_WARN_MS     200
#define BLINK_SEQUENZ_SPECIAL_CAN_PREOP_GERR_MS     200
#define BLINK_SEQUENZ_SPECIAL_CAN_OP_GERR_MS1       200
#define BLINK_SEQUENZ_SPECIAL_CAN_OP_GERR_MS2       1000
#define BLINK_SEQUENZ_SPECIAL_CAN_OP_SERR_MS1       200
#define BLINK_SEQUENZ_SPECIAL_CAN_OP_SERR_MS2       1000
#define BLINK_SEQUENZ_SPECIAL_CAN_OP_WARN_MS1       200
#define BLINK_SEQUENZ_SPECIAL_CAN_OP_WARN_MS2       1000
#define BLINK_SEQUENZ_SPECIAL_CAN_PREOP_SERR_MS1    200


typedef struct{
    tMultiPeriode * periods;
}tMultiSequence;

#define MULTI_SEQUENCE_STOP 0
#define MULTI_SEQUENCE_RESTART 1

typedef int tSequenceEnd;

void MULTI_InitSequence(tMultiSequence * sequence);
void MULTI_DeinitSequence(tMultiSequence * sequence);

tMultiPeriode * MULTI_NewPeriod(uint16_t periods,
                                tLedStaticColor  staticColor,
                                tTimeMS  timeBase);

void MULTI_FreeMulti(tMultiPeriode * pMulti);

tMultiElement * MULTI_NewElement(tLedColors color, tTimeMS change);
tMultiPeriode * MULTI_CreateSpecialSeq(tMultiParams * mp);

void MULTI_AddElementToPeriod(tMultiPeriode *,tMultiElement *);
void MULTI_AddPeriodToSequence(tMultiSequence *,tMultiPeriode *);
void MULTI_SetSequenceEnd(tMultiSequence *,tSequenceEnd);

#endif /* SEQ_SPECIAL_H_ */
//---- End of source file ------------------------------------------------------
