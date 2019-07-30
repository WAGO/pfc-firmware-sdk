//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file LedSchedule.c
///
/// \version $Revision: 35776 $
///
/// \brief the LED scheduler and related functions
///
/// \author Hans Florian Scholz : WAGO
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "common.h"
#include <ledserver_API.h>
#include "led_schedule.h"
#include "led_blink_sequential.h"
#include "sequence_handler.h"
#include <led_multi.h>
#include "leds.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
static pthread_cond_t   condLedSchedule;
static pthread_mutex_t  mutexLedSchedule = PTHREAD_MUTEX_INITIALIZER;
static pthread_t        threadLedSchedule;
tLedColors              LedColors[]={LED_OFF,//0
                                     LED_GREEN,//1
                                     LED_RED,//2
                                     LED_YELLOW//3
                                     };
tLED                  * SCHEDULE_led = NULL;
int                     ledScheduleRun = 0;

//-- Function: TimespecToTimeMS -----------------------------------------------------
///
/// Convert a struct timespec Value to a tTimeMS value
///
///  \param msDest destination of converted value
///  \param tsSrc  source that have to be converted
///
//------------------------------------------------------------------------------
static inline void TimespecToTimeMS(tTimeMS *msDest,const struct timespec *tsSrc)
{
  *msDest  = ((uint64_t)tsSrc->tv_sec) * MSEC_PER_SEC;
  *msDest += tsSrc->tv_nsec / NSEC_PER_MSEC;
}

//-- Function: LedSchedule -----------------------------------------------------
///
/// Convert a tTimeMS value to a struct timespec value
///
///  \param tsDest destination of converted value
///  \param msSrc  source that have to be converted
///
//------------------------------------------------------------------------------
static inline void TimeMSToTimespec(struct timespec *tsDest,const tTimeMS *msSrc)
{
  tsDest->tv_sec  = *msSrc / MSEC_PER_SEC;
  tsDest->tv_nsec = (*msSrc % MSEC_PER_SEC) * NSEC_PER_MSEC;
}

//-- Function: LedSchedule -----------------------------------------------------
///
/// The Main-Function of the Scheduler threat
///
///
/// \return NULL
//------------------------------------------------------------------------------
void *LedSchedule(void)
{

  struct timespec aktTime;
  tTimeMS        actMSec;

  pthread_mutex_lock(&mutexLedSchedule);

  clock_gettime(LED_SCHED_TIMER_BASE, &aktTime);
  TimespecToTimeMS(&actMSec,(const struct timespec*) &aktTime);

  //The mainloop
  while(ledScheduleRun > 0)
  {
    int             led_i;             //Iterator for LEDs
    struct timespec nextChange;
    tTimeMS        nextChangeMSec;

    //set nextChange to an unrealistic Time in future
    nextChangeMSec = END_OF_MSECONDS_64;
    
    //go through LEDs an do the blinking
    for(led_i = GetNoOfLeds()-1; led_i >= 0 ;led_i--)
    {
      pthread_mutex_lock(&SCHEDULE_led[led_i].mutexBlinkSeq);
      if(!SCHEDULE_led[led_i].active)
      {
    	pthread_mutex_unlock(&SCHEDULE_led[led_i].mutexBlinkSeq);
        continue;
      }

      //is it time to change for he LED?
      if(SCHEDULE_led[led_i].nextChange <= actMSec)
      {
        SEQUENTIAL_LedBlinkSequential( (uint16_t)led_i, &actMSec);
      }
      //set next time to wake up for the thread
      if(SCHEDULE_led[led_i].nextChange < nextChangeMSec)
      {
        nextChangeMSec = SCHEDULE_led[led_i].nextChange;
      }
      pthread_mutex_unlock(&SCHEDULE_led[led_i].mutexBlinkSeq);
    }//for
    
    //if nothing changed. no LED is blining and the thread can stop
    if(nextChangeMSec == END_OF_MSECONDS_64)
    {
      ledScheduleRun = -1;
    }
    else
    {    
      int rc = 0;
      //wait for next event
      TimeMSToTimespec(&nextChange, (const tTimeMS*) &nextChangeMSec);
      do {
      rc = pthread_cond_timedwait(&condLedSchedule,
                                  &mutexLedSchedule,
                                  &nextChange);
      clock_gettime(LED_SCHED_TIMER_BASE, &aktTime);
      TimespecToTimeMS(&actMSec,(const struct timespec*) &aktTime);
      }while(actMSec < nextChangeMSec && rc == 0);
    }

    
  }//main-loop
  
  pthread_mutex_unlock(&mutexLedSchedule);
  pthread_exit(NULL);
}


void SCHEDULE_LedScheduleSignal(void)
{
  static int     first = 0;//needed for the first call of this function

  DBGFUNC();
  if(!first)
  {
    pthread_condattr_t attr;
    first = 0xff;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, LED_SCHED_TIMER_BASE);
    pthread_cond_init(&condLedSchedule,&attr);
    pthread_mutex_init(&mutexLedSchedule,NULL);
  }

  pthread_mutex_lock(&mutexLedSchedule);

  //if mutex does not exist, we need to start the tread
  if(ledScheduleRun <= 0)
  {
    pthread_attr_t attr;
    struct sched_param scheduling_parameter;

    DBG("start thread");

    //prepare for starting the thread
    pthread_attr_init(&attr);   //attr Initialisieren
    if(pthread_attr_setstacksize(&attr, 16 * 1024))
    {
      pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    }
    pthread_attr_setinheritsched(&attr,LED_SCHED_INHERIT);
    pthread_attr_setschedpolicy(&attr, LED_SCHED_POLICY);

    scheduling_parameter.sched_priority = LED_SCHED_PRIORITY;
    pthread_attr_setschedparam(&attr, &scheduling_parameter);


    //thread must stop without calling pthread_join()
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    ledScheduleRun = 1;

    //start thread
    pthread_create(&threadLedSchedule, &attr,(void * )LedSchedule,NULL);
#ifdef HAVE_PTHREAD_SETNAME_NP
    pthread_setname_np(threadLedSchedule, "LED-Blink");
#endif
  }
  else
  {
    DBG("send condition");
    //send signal and unlock mutex
    pthread_cond_signal(&condLedSchedule);
  }
  pthread_mutex_unlock(&mutexLedSchedule);

}

static tLedReturnCode _ResetLed(tLedNr ledNr)
{

  tLedSchedHandler * resetHandler = NULL;
  tLedReturnCode ret = LED_RETURN_OK;
  void * oldData = NULL;
  void * oldUserData = NULL;
  if(LED_RETURN_OK == SEQHANDLER_GetSequenceHandler(SCHEDULE_led[ledNr].state,&resetHandler))
  {
    pthread_mutex_lock(&SCHEDULE_led[ledNr].mutexBlinkSeq);
    SCHEDULE_led[ledNr].active = 0;
    SCHEDULE_led[ledNr].state = LED_STATE_OFF;
    oldData = SCHEDULE_led[ledNr].data;
    oldUserData = SCHEDULE_led[ledNr].user_data;
    if(SCHEDULE_led[ledNr].pMulti != NULL)
    {
      MULTI_FreeMulti(SCHEDULE_led[ledNr].pMulti);
      SCHEDULE_led[ledNr].pMulti    = NULL;
      SCHEDULE_led[ledNr].pMultiAct = NULL;
    }
    SCHEDULE_led[ledNr].nextChange = 0;
    SCHEDULE_led[ledNr].blinkcounter = 0;
    DelLed((uint8_t)ledNr);
    pthread_mutex_unlock(&SCHEDULE_led[ledNr].mutexBlinkSeq);
    if(resetHandler->resetLedHandler != NULL)
    {
      if(0 != resetHandler->resetLedHandler(ledNr,oldData,oldUserData))
      {
        ret = LED_RETURN_ERROR_RESET_LED;
      }
    }
    if(resetHandler->resetUserDataHandler != NULL && oldUserData != NULL)
    {
      if(0 != resetHandler->resetUserDataHandler(ledNr,oldData,oldUserData) && ret != LED_RETURN_OK)
      {
        ret = LED_RETURN_ERROR_RESET_LED_USER_DATA;
      }
    }
  }
  return ret;
}

tLedReturnCode ledserver_LEDCTRL_SetLed(tLedNr ledNr, tLedStateClass status, void * data,void * user_data)
{
  tLedReturnCode ret;
  tLedSchedHandler * setHandler = NULL;

  if((uint8_t) ledNr >= GetNoOfLeds())
  {
    return LED_RETURN_ERROR_NUMBER;
  }
  do{
    ret = SEQHANDLER_GetSequenceHandler(status,&setHandler);
    if(ret != LED_RETURN_OK)break;
    ret = _ResetLed(ledNr);
    if(0 != setHandler->setLedHandler(ledNr,data,user_data))
    {
      if(ret == LED_RETURN_OK)
      {
        ret = LED_RETURN_ERROR_SET_LED;
      }
      //no break
    }
    else
    {
      SCHEDULE_led[ledNr].user_data = user_data;
      SCHEDULE_led[ledNr].state = status;
      if(SCHEDULE_led[ledNr].active)
      {
        SCHEDULE_LedScheduleSignal();
      }
    }
  }while(0);


  return ret;
}

tLedReturnCode ledserver_LEDCTRL_SetLedByName(const char * ledName, tLedStateClass status, void * data,void * user_data)
{
  tLedReturnCode ret;
  tLedNr  ledNr;

  ret = ledserver_LEDCTRL_GetLedNumber(ledName,&ledNr);
  if(ret == LED_RETURN_OK)
  {
    ret = ledserver_LEDCTRL_SetLed(ledNr,status,data,user_data);
  }
  return ret;
}

//-- Function: InitLeds --------------------------------------------------------
///
/// Initiate LED-Structures to get defined start-values
///
///
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_Init(void)
{
  int lednr;
  struct timeval tv;

  DBGFUNC();

  gettimeofday(&tv, NULL);
  //init LED-Hardware
  InitLed();
  if(SCHEDULE_led == NULL)
  {
    SCHEDULE_led = malloc(sizeof(tLED) * GetNoOfLeds());
    memset(SCHEDULE_led,0,sizeof(tLED) * GetNoOfLeds());
  }
  if(SCHEDULE_led == NULL)
  {
    fprintf(stderr,"ERROR: could not alloc led memory\n");
    exit(EXIT_FAILURE);
  }

  //set all LED off
  for(lednr=0; lednr < GetNoOfLeds(); lednr++)
  {
    SCHEDULE_led[lednr].state              = LED_STATE_STATIC;
    SCHEDULE_led[lednr].nextChange         = 0;
    SCHEDULE_led[lednr].blinkcounter       = 0;
    SCHEDULE_led[lednr].pMulti             = NULL;
    SCHEDULE_led[lednr].pMultiAct          = NULL;
    SCHEDULE_led[lednr].data               = NULL;
    SCHEDULE_led[lednr].szData             = 0;
    SCHEDULE_led[lednr].szDataBuffer       = 0;
    SCHEDULE_led[lednr].user_data          = NULL;
    pthread_mutex_init(&SCHEDULE_led[lednr].mutexBlinkSeq,NULL);
    DelLed(lednr);
  }
  return LED_RETURN_OK;
}



//-- Function: SCHEDULE_DeinitLeds --------------------------------------------------------
///
/// Initiate LED-Structures to get defined start-values
///
///
//------------------------------------------------------------------------------
void ledserver_LEDCTRL_Deinit(void)
{
  int lednr;

  ledScheduleRun = 0;
  //set all LED off
  for(lednr=0; lednr < GetNoOfLeds(); lednr++)
  {
    _ResetLed(lednr);
  }

}

//-- Function: SCHEDULE_SetNextChangeTime --------------------------------------
///
///  Calculate the next time for changing the LED
///
/// \param ledNextChange Pointer to the variable where tha value has to be written in
/// \param nextChangeMs  absolut time distance for waiting for the next change
/// \param timebaseMs    value of timebase where the waiting time will be setted to
/// \param aktTime       actuel time
///
//------------------------------------------------------------------------------
void SCHEDULE_SetNextChangeTime(tTimeMS       *ledNextChange,
                                const tTimeMS nextChangeMs,
                                const tTimeMS timebaseMs,
                                const tTimeMS *aktTime)
{
  //calculate the real time when the last led change had to be done
  tTimeMS lastPointOfTimebase = timebaseMs * (*aktTime / timebaseMs);
  //calculate the next time for change
  *ledNextChange = lastPointOfTimebase + nextChangeMs;
}

tLedReturnCode ledserver_LEDCTRL_GetLed(tLedNr ledNr, tLedStateClass * status, void * data, size_t szData,void ** user_data)
{
  tLedReturnCode ret;
  tLedSchedHandler * getHandler = NULL;
  if((uint8_t) ledNr >= GetNoOfLeds())
  {
    return LED_RETURN_ERROR_NUMBER;
  }
  if(status == NULL)
  {
    return LED_RETURN_ERROR_PARAMETER;
  }
  if(szData < SCHEDULE_led[ledNr].szData)
  {
    return LED_RETURN_ERROR_BUFFERSIZE;
  }
  if(data == NULL)
  {
    return LED_RETURN_ERROR_PARAMETER;
  }
  //*data = SCHEDULE_led[ledNr].data;
  memcpy(data,SCHEDULE_led[ledNr].data,SCHEDULE_led[ledNr].szData);
  if(user_data != NULL)
  {
    *user_data = SCHEDULE_led[ledNr].user_data;
  }
  *status = SCHEDULE_led[ledNr].state;
  ret = SEQHANDLER_GetSequenceHandler(SCHEDULE_led[ledNr].state,&getHandler);

  if(ret == LED_RETURN_OK)
  {
    if(getHandler->getLedHandler != NULL &&  data != NULL)
    {
      if(0 != getHandler->getLedHandler(ledNr,data,user_data))
      {
        ret = LED_RETURN_ERROR_GET_LED;
      }
    }
  }

  return ret;
}

tLedReturnCode ledserver_LEDCTRL_GetLedByName(const char * ledName, tLedStateClass * status, void * data, size_t szData,void ** user_data)
{
  tLedReturnCode ret;
  tLedNr  ledNr;

  ret = ledserver_LEDCTRL_GetLedNumber(ledName,&ledNr);
  if(ret == LED_RETURN_OK)
  {
    ret = ledserver_LEDCTRL_GetLed(ledNr,status,data,szData,user_data);
  }
  return ret;
}

tLedReturnCode ledserver_LEDCTRL_GetLedName(tLedNr ledNr, char * ledNameBuffer,size_t szLedNameBuffer)
{
  char buffer[16];
  if(ledNameBuffer == NULL)
  {
    return LED_RETURN_ERROR_PARAMETER;
  }
  if(0 != GetLedBaseName((uint8_t) ledNr, buffer))
  {
    return LED_RETURN_ERROR_NUMBER;
  }
  strncpy(ledNameBuffer,buffer,szLedNameBuffer);
  return LED_RETURN_OK;

}
tLedReturnCode ledserver_LEDCTRL_GetLedNumber(const char * ledName,tLedNr * pLedNr)
{
  tLedNr ledNr;
  tLedReturnCode ret = LED_RETURN_ERROR_NAME;
  char buffer[16];
  if(pLedNr == NULL)
  {
    return LED_RETURN_ERROR_PARAMETER;
  }
  if(ledName == NULL)
  {
    return LED_RETURN_ERROR_PARAMETER;
  }
  GetLedBaseName((uint8_t) 0, buffer);
  if(!strcmp(buffer,"GPIO"))
  {
    return LED_RETURN_ERROR_NO_NAME_AVAILABLE;
  }
  for(ledNr = 0; ledNr < GetNoOfLeds(); ledNr++)
  {

    GetLedBaseName((uint8_t) ledNr, buffer);
    if(!strcasecmp(ledName, buffer))
    {
      ret = LED_RETURN_OK;
      *pLedNr = ledNr;
      break;
    }
  }
  return ret;
}

tLedNr ledserver_LEDCTRL_GetLedCount(void)
{
  return (tLedNr) GetNoOfLeds();
}

void ledserver_LEDCTRL_SetLedStateData(tLedNr ledNr,void * data,size_t szData)
{
  if(szData>SCHEDULE_led[ledNr].szDataBuffer)
  {
    SCHEDULE_led[ledNr].data = realloc(SCHEDULE_led[ledNr].data,szData);
    SCHEDULE_led[ledNr].szDataBuffer = szData;
  }
  memcpy(SCHEDULE_led[ledNr].data,data,szData);
  SCHEDULE_led[ledNr].szData = szData;
}

int LEDHANDLER_SetStatic(tLedNr ledNr, void * data,void * user_data)
{
  tLedStatic* staticLed = (tLedStatic*) (data);
  (void)user_data;

  ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLedStatic));

  return SetLed(ledNr,(uint8_t)*staticLed);
}


//---- End of source file ------------------------------------------------------

