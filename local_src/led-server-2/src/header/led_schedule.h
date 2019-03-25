//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file led_schedule.h
///
/// \version $Revision: 13630 $
///
/// \brief header for LED-scheduler
///
/// \author <author name> : <company name>
//------------------------------------------------------------------------------

#ifndef _LED_SCHEDULE_H
#define _LED_SCHEDULE_H

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdint.h>
#include <pthread.h>
#include <ledserver_API.h>

//#include "LedCtl_API.h"
#include "leds.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define LED_STAT_FILE "/tmp/led.stat"

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC 1000
#endif

#ifndef NSEC_PER_MSEC
#define NSEC_PER_MSEC 1000000
#endif

#define END_OF_SECONDS 0x7FffFFff
#define END_OF_MSECONDS_64 0xFFFFFFFFffffffff
#define END_OF_NSECONDS 0x7FffFFff

#define LED_SCHED_TIMER_BASE CLOCK_MONOTONIC  /* The monotonic clock is unmodifiable */
//#define LED_SCHED_TIMER_BASE CLOCK_REALTIME

#define LED_SCHED_INHERIT    PTHREAD_EXPLICIT_SCHED
#define LED_SCHED_POLICY     SCHED_OTHER
#define LED_SCHED_PRIORITY   0

//#define LED_FLAG_DEFAULT_VAL 0x01
//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef uint64_t tTimeMS;
#if 0
typedef union {
    uint8_t  byte;
    uint16_t word;
    uint32_t integer;
}tUDataType;

typedef struct {
    int        type;
    tUDataType value;
}tIdAdditionalParam;

typedef struct {
    uint32_t   id;
    GList    * info;
    uint32_t   refCount;
}tIdInfo;

typedef struct stWarteschlange{
    tIdInfo         idInfo;
    uint16_t        var1;              /* first Blinkkargument Errorcode */
    uint16_t        var2;              /* second Blinkargiment Errorarg */
    struct timeval  setTime;
    struct stWarteschlange *pNext;
}tWarteschlange;
#endif

typedef struct stMultiElement{
    tLedColors  color;
    tTimeMS     change;
    struct stMultiElement * pNext;
}tMultiElement;

typedef struct stMultiPeriode{
    uint16_t                periods;
    tLedStaticColor         staticColor;
    tTimeMS                 timeBase;
    tMultiElement         * list;
    tMultiElement         * pAct;
    struct stMultiPeriode * pNext;
}tMultiPeriode;

typedef struct stLED{
//  uint16_t        var1;               /* First Blinkargument */
//  uint16_t        var2;               /* Second Argument */
//  tWarteschlange *pWarteschlange;     /* waiting-queue pointer */
    tLedStateClass         state;              /* actuel state of LED */
//  uint8_t         defaultState;
//  uint16_t        defaultVar1;
//  uint16_t        defaultVar2;
//  tIdInfo         defaultIdInfo;
//  uint8_t         blinkstate;         /* State of Blink-Statemachine*/
  uint8_t         blinkcounter;       /* counter for BLINK_SEQ*/
  tTimeMS         nextChange;         /* time until next Change */
//  tIdInfo         idInfo;
//  uint8_t         flags;
//  struct timeval  setTime;
  tMultiPeriode * pMulti;
  tMultiPeriode * pMultiAct;
  pthread_mutex_t mutexBlinkSeq;
  void * data;
  size_t szData;
  size_t szDataBuffer;
  void * user_data;
  int  active;
}tLED;



//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
extern tLED         * SCHEDULE_led;
extern tLedColors     LedColors[];

#if 0
int SCHEDULE_SetLedState(uint16_t ledNo,
                         uint8_t  state,
                         uint16_t var1,
                         uint16_t var2,
                         tIdInfo * id,
                         struct timeval * setTime);

int SCHEDULE_SetLedDefaultState(uint16_t,
                                uint8_t ,
                                uint16_t,
                                uint16_t,
                                tIdInfo*);

int SCHEDULE_SetLedDefaultStatebyName(char*,
                                      uint8_t,
                                      uint16_t,
                                      uint16_t,
                                      tIdInfo*);
#endif
void SCHEDULE_LedScheduleSignal(void);

void SCHEDULE_InitLeds(void);

void SCHEDULE_DeinitLeds(void);

void SCHEDULE_WriteLedStat(void);


void SCHEDULE_SetNextChangeTime(tTimeMS       *ledNextChange,
                                const tTimeMS nextChangeMs,
                                const tTimeMS timebaseMs,
                                const tTimeMS *aktTime);

#if 0
uint8_t SCHEDULE_GetLedState(uint16_t ledNr,
                             uint16_t *var1,
                             uint16_t *var2,
                             tIdInfo  *id,
                             struct timeval *tv);

int     SCHEDULE_GetLedInfo( uint16_t  ledNr,
                             tLED    * ledInfo);
int     SCHEDULE_SetLedFlags(uint16_t  ledNr,
                             uint8_t   flags);

void SCHEDULE_SetDefaultState(uint16_t ledNr);
void SCHEDULE_CleanIdInfo(tIdInfo * idInfo);
void SCHEDULE_SetIdInfo(uint16_t ledNr,tIdInfo * idInfo);
#endif
int SCHEDULE_GetLedByName(char*);
#endif
