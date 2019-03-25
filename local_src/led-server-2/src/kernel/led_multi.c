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
///  \file     seq_special.c
///
///  \version  $Revision: 12545 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include "common.h"
#include "led_schedule.h"
#include "led_multi.h"
#include <malloc.h>

void MULTI_InitSequence(tMultiSequence * sequence)
{
  sequence->periods = NULL;
}

void MULTI_DeinitSequence(tMultiSequence * sequence)
{
  MULTI_FreeMulti(sequence->periods);
}

tMultiPeriode * MULTI_NewPeriod(uint16_t periods,
                                tLedStaticColor  staticColor,
                                 tTimeMS  timeBase)
{
  tMultiPeriode * ret = malloc(sizeof(tMultiPeriode));
  DBGFUNC();
  ret->periods = periods;
  ret->staticColor = staticColor;
  ret->timeBase = timeBase;
  ret->pNext = NULL;
  ret->list = NULL;
  ret->pAct = NULL;
  return ret;
}

void FreeElements(tMultiElement * pElAct)
{
  while(pElAct != NULL)
  {
    tMultiElement *pDelEl = pElAct;
    pElAct = pElAct->pNext;
    free(pDelEl);
  }
}

void MULTI_FreeMulti(tMultiPeriode * pMulti)
{
  tMultiPeriode * pMultiOrig = pMulti;
  tMultiPeriode * pDelPe   = pMulti;
  if(pMulti == NULL)
  {
    return;
  }
  FreeElements(pMulti->list);
  pMulti = pMulti->pNext;
  free(pDelPe);
  while(  (pMulti != pMultiOrig)
        &&(pMulti != NULL))
  {
    tMultiPeriode * pDelPe   = pMulti;

    FreeElements(pMulti->list);
    pMulti = pMulti->pNext;
    free(pDelPe);
  }
}

tMultiElement * MULTI_NewElement(tLedColors color, tTimeMS change)
{
  tMultiElement * ret = malloc(sizeof(tMultiElement));
  DBGFUNC();
  ret->color = color;
  ret->change = change;
  ret->pNext = NULL;
  return ret;
}

void MULTI_AddElementToPeriod(tMultiPeriode *period,tMultiElement *element)
{
  if(period == NULL || element == NULL)
  {
    return;
  }
  if(period->list == NULL)
  {
    period->list = element;
    period->pAct = element;
  }
  else
  {
    tMultiElement * pAct = period->list;
    while(pAct->pNext != NULL)
    {
      pAct = pAct->pNext;
    }
    pAct->pNext = element;
  }
}
void MULTI_AddPeriodToSequence(tMultiSequence * sequence,tMultiPeriode * period)
{
  if(period == NULL || sequence == NULL)
  {
    return;
  }
  if(sequence->periods == NULL)
  {
    sequence->periods = period;
  }
  else
  {
    tMultiPeriode * pAct = sequence->periods;
    while(pAct->pNext != NULL)
    {
      pAct = pAct->pNext;
    }
    pAct->pNext = period;
  }
}
void MULTI_SetSequenceEnd(tMultiSequence *sequence ,tSequenceEnd end)
{

  if(sequence == NULL)
  {
    return;
  }
  if(end == MULTI_SEQUENCE_RESTART)
  {
    tMultiPeriode * pAct = sequence->periods;
    while(pAct->pNext != NULL)
    {
      pAct = pAct->pNext;
    }
    pAct->pNext = sequence->periods;
  }
}

//---- End of source file ------------------------------------------------------
