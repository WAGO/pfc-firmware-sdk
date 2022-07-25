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
///  \file     led_blink_sequential.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LED_BLINK_SEQUENTIAL_H_
#define LED_BLINK_SEQUENTIAL_H_

#include <ledserver_API.h>
#include "led_multi.h"

/*void SEQUENTIAL_LedBlinkSequential(uint16_t ledNo,
                                   struct timespec  *aktTime);*/
void SEQUENTIAL_LedBlinkSequential(uint16_t ledNo,
                                   tTimeMS  *aktTime);

void SEQUENTIAL_DelFullQueue(uint16_t ledNo);

tLedReturnCode SEQUENTIAL_SetBlinkSequence(tLedNr ledNr,
                                           tMultiSequence * sequence);
#if 0
int SEQUENTIAL_SetBlinkSequenz(uint16_t ledNo,
                               tIdInfo * id,
                               uint8_t state,
                               uint16_t var1,
                               uint16_t var2,
                               struct timeval * setTime);


void SEQUENTIAL_DelQueueElementByEventId(uint16_t ledNo, tIdInfo * id);
#endif

#endif /* LED_BLINK_SEQUENTIAL_H_ */
//---- End of source file ------------------------------------------------------
