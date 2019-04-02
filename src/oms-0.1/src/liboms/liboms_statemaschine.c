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
///  \file     liboms_statemaschine.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "liboms_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <string.h>

#define OMS_STATE_MASK            0x01
#define OMS_STATE_UP              0x02
#define OMS_STATE_TIMEOUT         0x04
#define OMS_STATE_NOT_TIMEOUT     0x08
#define OMS_STATE_NEED_ACTIVE     0x10
#define OMS_STATE_NOT_ACTIVE      0x20

tOmsStateSwitch *rootState = NULL;


#if 0
void oms_SM_PrintSwitches(void)
{
  tOmsStateSwitch * pAct = rootState;
  int i = 0;
  while(pAct != NULL)
  {
    printf("\n%d                    = %X\n",i, (int)pAct);
    printf("%d.MaskWatch          = 0x%.2X\n",i, pAct->MaskWatch);
    printf("%d.MaskState          = 0x%.2X\n",i, pAct->MaskState);
    printf("%d.flagsWatch         = 0x%.2X\n",i, pAct->flagsWatch);
    printf("%d.timeoutMsec        = %d\n",i, pAct->timeoutMsec);
    printf("%d.flagsState         = 0x%.2X\n",i, pAct->flagsState);
    printf("%d.maschineStateFlags = 0x%.2X\n",i, pAct->maschineStateFlags);
    printf("%d.callFunc           = 0x%X\n",i,(int) pAct->callFunc);
    printf("%d.callStates         = 0x%X\n",i, (int)pAct->callStates);
    if(pAct->callStates != NULL)
    {
      tOmsStateCallList *pStates = pAct->callStates;
      while(pStates != NULL)
      {
        printf(" -> %X\n", (int) pStates->sSwitch);
        pStates = pStates->pNext;
      }
    }
    printf("%d.NotActive          = 0x%X\n",i, (int) pAct->callNotActive);
    if(pAct->callNotActive != NULL)
    {
      tOmsStateCallList *pStates = pAct->callNotActive;
      while(pStates != NULL)
      {
        printf(" -> %X\n", (int) pStates->sSwitch);
        pStates = pStates->pNext;
      }
    }
    printf("%d.pNext              = 0x%X\n",i, (int) pAct->pNext);
    i++;
    pAct = pAct->pNext;
  }
}
#endif
void oms_SM_CleanStateSwitch(tOmsStateSwitch * pSwitch)
{
  memset(pSwitch,0, sizeof(tOmsStateSwitch));
  pSwitch->callFunc = NULL;
  pSwitch->callStates = NULL;
  pSwitch->callNeedActive = NULL;
  pSwitch->callNotActive = NULL;
  pSwitch->pNext = NULL;
}

tOmsStateSwitch * oms_SM_NewStateSwitch(int StartElement)
{
  tOmsStateSwitch * new = NULL;

  new = malloc(sizeof(tOmsStateSwitch));
  if(new != NULL)
  {
    oms_SM_CleanStateSwitch(new);
    if(StartElement)
    {
      new->maschineStateFlags |=  OMS_MSTATE_ALLWAYS_ACTIVE;
    }
  }

  return new;
}
/*which keys and switches shoult be watched*/
tOmsReturn oms_SM_SetWatchMask(tOmsStateSwitch * pSwitch, tOmsState watch, tOmsState values)
{
  if(~watch & values)
  {
    //there are values set which not will be watched
    return OMS_RETURN_ERROR;
  }
  pSwitch->MaskState = values;
  pSwitch->MaskWatch = watch;
  pSwitch->flagsWatch |= OMS_WATCH_MASK;
  return OMS_RETURN_OK;
}

/*Set timeout. if negative value is given the timeout make the maximum time to switch state*/
tOmsReturn oms_SM_SetTimeout(tOmsStateSwitch * pSwitch, int timeout)
{
  if(timeout >= 0)
  {
    pSwitch->timeoutMsec = timeout;
    pSwitch->flagsWatch |= OMS_WATCH_TIMEOUT;
    pSwitch->flagsWatch &= ~OMS_WATCH_NOT_TIMEOUT;
  }
  else
  {
    pSwitch->timeoutMsec = -timeout;
    pSwitch->flagsWatch |= OMS_WATCH_NOT_TIMEOUT;
    pSwitch->flagsWatch &= ~OMS_WATCH_TIMEOUT;
  }
  return OMS_RETURN_OK;
}

/*Set OMS_WATCH_UP bit for stateswitch after leaving buttom*/
tOmsReturn oms_SM_SetOnButtomUp(tOmsStateSwitch * pSwitch,int up)
{
  if(up)
  {
    pSwitch->flagsWatch |= OMS_WATCH_UP;
  }
  else
  {
    pSwitch->flagsWatch &= ~OMS_WATCH_UP;
  }
  return OMS_RETURN_OK;
}

tOmsReturn oms_SM_SetCallFunction(tOmsStateSwitch * pSwitch,void (*func)(void *), void * args)
{
  tOmsEventFunctionCall * new = malloc(sizeof(tOmsEventFunctionCall));
  if(new == NULL)
  {
    return OMS_RETURN_ERROR;
  }
  new->func = func;
  new->args = args;
  new->pNext = pSwitch->callFunc;
  pSwitch->callFunc = new;
  return OMS_RETURN_OK;
}

tOmsReturn oms_SM_RegisterSwitch(tOmsStateSwitch *pSwitch)
{
  //check if switch is already registered!!!
  if(pSwitch == NULL)
  {
    return OMS_RETURN_ERROR;
  }
  if(rootState == NULL)
  {
    rootState = pSwitch;
  }
  else
  {
    tOmsStateSwitch * pAct = rootState;
    do{
      if(pAct == pSwitch)
      {
        break;
      }
      else if(pAct->pNext == NULL)
      {
        pAct->pNext = pSwitch;
        //make sure that we have all switches in list registered by this function!!!
        pSwitch->pNext = NULL;
        break;
      }
      pAct = pAct->pNext;
    }while(pAct != NULL);
  }
  //pSwitch->pNext = rootState;
  //rootState = pSwitch;
  return OMS_RETURN_OK;
}

tOmsReturn oms_SM_SetFollowSwitch(tOmsStateSwitch *pSwitch, tOmsStateSwitch *pFollow)
{
  tOmsStateCallList * new = NULL;
  if(pFollow->maschineStateFlags & OMS_MSTATE_ALLWAYS_ACTIVE)
  {
    puts("ERROR MFLAGS");
    return OMS_RETURN_ERROR;
  }
  new = malloc(sizeof (tOmsStateCallList));
  if(new == NULL)
  {
    puts("ERROR NEW_FOLLOW");
    return OMS_RETURN_ERROR;
  }
  new->sSwitch = pFollow;
  new->pNext = pSwitch->callStates;
  pSwitch->callStates = new;

  return OMS_RETURN_OK;
}

tOmsReturn oms_SM_SetNeedActiveSwitch(tOmsStateSwitch *pSwitch,
                                      tOmsStateSwitch *pActive)
{
  tOmsStateCallList * new = NULL;
  new = malloc(sizeof (tOmsStateCallList));
  if(new == NULL)
  {
    return OMS_RETURN_ERROR;
  }

  pSwitch->flagsWatch |= OMS_WATCH_NEED_ACTIVE;
  new->sSwitch = pActive;
  new->pNext = pSwitch->callNeedActive;
  pSwitch->callNeedActive = new;

  return OMS_RETURN_OK;
}
tOmsReturn oms_SM_SetNotActiveSwitch(tOmsStateSwitch *pSwitch,
                                     tOmsStateSwitch *pNotActive)
{
  tOmsStateCallList * new = NULL;
  new = malloc(sizeof (tOmsStateCallList));
  if(new == NULL)
  {
    return OMS_RETURN_ERROR;
  }
  pSwitch->flagsWatch |= OMS_WATCH_NOT_ACTIVE;
  new->sSwitch = pNotActive;
  new->pNext = pSwitch->callNotActive;
  pSwitch->callNotActive = new;

  return OMS_RETURN_OK;
}

/* Calculate time between actual time and given time in past*/
int _DiffTimeMsec(struct timeval * occur)
{
  int ret = 0;
  struct timeval diff;

  diff.tv_sec = eventTime.tv_sec - occur->tv_sec;

  if(eventTime.tv_usec < occur->tv_usec )
  {
    diff.tv_sec--;
  }
  diff.tv_usec = abs((int)eventTime.tv_usec - (int)occur->tv_usec);

  ret = diff.tv_usec /1000;
  ret += diff.tv_sec * 1000;

  return ret;
}

bool _CheckNotActive(tOmsStateSwitch *pActState)
{
  int ret = true;
  tOmsStateCallList * pAct = pActState->callNotActive;
  while(pAct)
  {
    if(pAct->sSwitch->maschineStateFlags & OMS_MSTATE_ACTIVE)
    {
      ret = false;
      break;
    }
    pAct = pAct->pNext;
  }

  return ret;
}
bool _CheckNeedActive(tOmsStateSwitch *pActState)
{
  int ret = true;
  tOmsStateCallList * pAct = pActState->callNeedActive;
  while(pAct)
  {
    if(!(pAct->sSwitch->maschineStateFlags & OMS_MSTATE_ACTIVE))
    {
      ret = false;
      break;
    }
    pAct = pAct->pNext;
  }

  return ret;
}

/* Check if StateSwitch is fullfilled */
int _UpdateState(tOmsDevice * dev, tOmsStateSwitch *pActState)
{
  int ret = -1;
  if(   !(pActState->flagsState & OMS_STATE_MASK)
      && (    (pActState->MaskWatch & pActState->MaskState)
          == (pActState->MaskWatch & dev->bottons)))
   {
     pActState->flagsState |= OMS_STATE_MASK;
     if(!(pActState->maschineStateFlags & OMS_MSTATE_TIME_SET))
     {
       pActState->timeOccured.tv_sec = eventTime.tv_sec;
       pActState->timeOccured.tv_usec = eventTime.tv_usec;
       pActState->maschineStateFlags |= OMS_MSTATE_TIME_SET;
     }
   }

   if(pActState->flagsWatch & (OMS_WATCH_TIMEOUT | OMS_WATCH_NOT_TIMEOUT))
   {
     int timeLeft = _DiffTimeMsec(&pActState->timeOccured);
     if(pActState->timeoutMsec <= timeLeft)
     {
       pActState->flagsState |= OMS_STATE_TIMEOUT;
       pActState->flagsState &= ~(OMS_STATE_NOT_TIMEOUT);
     }
     else
     {
       int posibleNextTimeout = pActState->timeoutMsec -timeLeft;
       pActState->flagsState |= OMS_STATE_NOT_TIMEOUT;
       ret = posibleNextTimeout;
     }
   }
   if(pActState->flagsWatch & OMS_WATCH_UP)
   {
     if((    (pActState->MaskWatch & pActState->MaskState)
        != (pActState->MaskWatch & dev->bottons)))
     {
       pActState->flagsState |= OMS_STATE_UP;
     }
   }
   if(pActState->flagsWatch & OMS_WATCH_NOT_ACTIVE)
   {
     if(_CheckNotActive(pActState))
     {
       pActState->flagsState |= OMS_STATE_NOT_ACTIVE;
     }
   }
   if(pActState->flagsWatch & OMS_WATCH_NEED_ACTIVE)
   {
     if(_CheckNeedActive(pActState))
     {
       pActState->flagsState |= OMS_STATE_NEED_ACTIVE;
     }
   }
   if(pActState->flagsWatch == pActState->flagsState)
   {
     //pActState->call->func(pActState->call->args);
     pActState->maschineStateFlags |= OMS_MSTATE_READY;
     pActState->flagsState = 0;
   }

   if(     (pActState->flagsState & OMS_STATE_MASK)
        && (    (pActState->MaskWatch & pActState->MaskState)
            != (pActState->MaskWatch & dev->bottons)))
   {
     pActState->flagsState = 0;
     pActState->maschineStateFlags &= ~(OMS_MSTATE_ACTIVE | OMS_MSTATE_TIME_SET);
   }
   if(!(pActState->flagsState & OMS_STATE_MASK))
   {
     pActState->flagsState = 0;
   }
   else
   {
     pActState->flagsState = OMS_STATE_MASK;
   }
  return ret;
}

/* Set the following states of an fullfilled state into active state */
inline static int _SetStatesActive(tOmsStateSwitch *pActState)
{
  tOmsStateCallList * pActSet = pActState->callStates;
  int ret = 0;

  while(pActSet != NULL)
  {
    pActSet->sSwitch->maschineStateFlags |= OMS_MSTATE_ACTIVE;
    if(!(pActSet->sSwitch->maschineStateFlags & OMS_MSTATE_TIME_SET))
    {
      pActSet->sSwitch->timeOccured.tv_sec = eventTime.tv_sec;
      pActSet->sSwitch->timeOccured.tv_usec = eventTime.tv_usec;
      pActSet->sSwitch->maschineStateFlags |= OMS_MSTATE_TIME_SET;
    }
    ret ++;
    pActSet = pActSet->pNext;
  }

  return ret;
}

/* Call All functions of a fullfilled state */
inline static void _CallStateFunctions(tOmsStateSwitch *pActState)
{
  tOmsEventFunctionCall * pActCall = pActState->callFunc;

  while(pActCall != NULL)
  {
    pActCall->func(pActCall->args);
    pActCall = pActCall->pNext;
  }
}

/* Check all stateswitches to be fullfilled */
static inline void _UpdateStates(tOmsDevice *dev,
                   tOmsStateSwitch * pActState,
                   tOmsMaschineStateFlags mstates,
                   int *retTimeout)
{
    while(pActState != NULL)
    {
      if(pActState->maschineStateFlags &  mstates)
      {
        int timeout;
        timeout = _UpdateState(dev, pActState);
        if(   (   (*retTimeout == -1)
               || (timeout < *retTimeout))
           && (timeout != -1))
        {
          *retTimeout = timeout;
        }
      }
      pActState = pActState->pNext;
    }
}

/*switch all fullfilled states into next state */
static inline int _DoSwitch(tOmsStateSwitch * pActState,
                            tOmsMaschineStateFlags mstates)
{
  int count = 0;
  while(pActState != NULL)
   {
     if(   (pActState->maschineStateFlags & mstates)
        && (pActState->maschineStateFlags & OMS_MSTATE_READY))
     {
       pActState->maschineStateFlags &= ~(OMS_MSTATE_ACTIVE | OMS_MSTATE_READY | OMS_MSTATE_TIME_SET);
       pActState->flagsState = 0;
       count += _SetStatesActive(pActState);
       _CallStateFunctions(pActState);
     }
     pActState = pActState->pNext;
   }

  return count;
}

int SM_UpdateStateMaschine(tOmsDevice * dev)
{
  int retTimeout = -1;

  _UpdateStates(dev,rootState,(OMS_MSTATE_ACTIVE | OMS_MSTATE_ALLWAYS_ACTIVE),&retTimeout);
  if(_DoSwitch(rootState,(OMS_MSTATE_ACTIVE | OMS_MSTATE_ALLWAYS_ACTIVE)))
  {
    do
    {
      _UpdateStates(dev,rootState,(OMS_MSTATE_ACTIVE),&retTimeout);
    }while(_DoSwitch(rootState,(OMS_MSTATE_ACTIVE)));
  }
  //call it again to be shure that all states are got
  _UpdateStates(dev,rootState,(OMS_MSTATE_ACTIVE),&retTimeout);
  _DoSwitch(rootState,(OMS_MSTATE_ACTIVE));

  return retTimeout;
}


//---- End of source file ------------------------------------------------------
