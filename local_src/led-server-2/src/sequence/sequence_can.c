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
///  \file     sequence_can.c
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
#include "common.h"
#include <ledserver_API.h>
//#include "sequence_can.h"

struct stSpecialCodesElements {
    tLedColors  color;
    tTimeMS     change;
};

struct stSpecialPeriod {
    uint16_t   periods;
    tLedColors stopColor;
    size_t     numElements;
    struct stSpecialCodesElements * elements;
};

struct stSpecialCodes {
    uint32_t   single;
    size_t     numElements;
    struct stSpecialPeriod * periods;
};

//Preopertional + Warning Level
static struct stSpecialCodesElements specialCanElements_preop_warn_p1[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_WARN_MS},
  {LED_OFF   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_WARN_MS},
};
static struct stSpecialCodesElements specialCanElements_preop_warn_p2[] =
{
  {LED_GREEN ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_WARN_MS},
  {LED_OFF   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_WARN_MS},
};

static struct stSpecialPeriod specialCanPeriods_preop_warn[] =
{
 {
  .periods     = 1,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_preop_warn_p1)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_preop_warn_p1,
 },
 {
   .periods     = 2,
   .stopColor   = LED_COLOR_OFF,
   .numElements = sizeof(specialCanElements_preop_warn_p2)/sizeof(struct stSpecialCodesElements),
   .elements    = specialCanElements_preop_warn_p2,
 }
};


//Preopertional + Guard Error
static struct stSpecialCodesElements specialCanElements_preop_gerr_p1[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_GERR_MS},
  {LED_OFF   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_GERR_MS},
};

static struct stSpecialCodesElements specialCanElements_preop_gerr_p2[] =
{
  {LED_GREEN ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_GERR_MS},
  {LED_OFF   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_GERR_MS},
};

static struct stSpecialPeriod specialCanPeriods_preop_gerr[] =
{
 {
  .periods     = 2,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_preop_gerr_p1)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_preop_gerr_p1,
 },
 {
   .periods     = 2,
   .stopColor   = LED_COLOR_OFF,
   .numElements = sizeof(specialCanElements_preop_gerr_p2)/sizeof(struct stSpecialCodesElements),
   .elements    = specialCanElements_preop_gerr_p2,
 }
};

//Operational + Guard Error
static struct stSpecialCodesElements specialCanElements_op_gerr_p1[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_OP_GERR_MS1},
  {LED_GREEN ,BLINK_SEQUENZ_SPECIAL_CAN_OP_GERR_MS1},
};
static struct stSpecialCodesElements specialCanElements_op_gerr_p2[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_OP_GERR_MS1},
  {LED_GREEN ,BLINK_SEQUENZ_SPECIAL_CAN_OP_GERR_MS2},
};

static struct stSpecialPeriod specialCanPeriods_op_gerr[] =
{
 {
  .periods     = 1,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_op_gerr_p1)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_op_gerr_p1,
 },
 {
  .periods     = 1,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_op_gerr_p2)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_op_gerr_p2,
 }
};

//Operational + Sync Error
static struct stSpecialCodesElements specialCanElements_op_serr_p1[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_OP_SERR_MS1},
  {LED_GREEN ,BLINK_SEQUENZ_SPECIAL_CAN_OP_SERR_MS1},
};
static struct stSpecialCodesElements specialCanElements_op_serr_p2[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_OP_SERR_MS1},
  {LED_GREEN ,BLINK_SEQUENZ_SPECIAL_CAN_OP_SERR_MS2},
};


static struct stSpecialPeriod specialCanPeriods_op_serr[] =
{
 {
  .periods     = 2,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_op_serr_p1)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_op_serr_p1,
 },
 {
   .periods     = 1,
   .stopColor   = LED_COLOR_OFF,
   .numElements = sizeof(specialCanElements_op_serr_p2)/sizeof(struct stSpecialCodesElements),
   .elements    = specialCanElements_op_serr_p2,
 }
};


//Opertional + Warning Level
static struct stSpecialCodesElements specialCanElements_op_warn[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_OP_WARN_MS1},
  {LED_GREEN   ,BLINK_SEQUENZ_SPECIAL_CAN_OP_WARN_MS2},
};

static struct stSpecialPeriod specialCanPeriods_op_warn[] =
{
 {
  .periods     = 1,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_op_warn)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_op_warn,
 }
};


//Preoperational + Sync Error
static struct stSpecialCodesElements specialCanElements_preop_serr_p1[] =
{
  {LED_RED   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_SERR_MS1},
  {LED_OFF   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_SERR_MS1},
};
static struct stSpecialCodesElements specialCanElements_preop_serr_p2[] =
{
  {LED_GREEN   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_SERR_MS1},
  {LED_OFF   ,BLINK_SEQUENZ_SPECIAL_CAN_PREOP_SERR_MS1},
};


static struct stSpecialPeriod specialCanPeriods_preop_serr[] =
{
 {
  .periods     = 3,
  .stopColor   = LED_COLOR_OFF,
  .numElements = sizeof(specialCanElements_preop_serr_p1)/sizeof(struct stSpecialCodesElements),
  .elements    = specialCanElements_preop_serr_p1,
 },
 {
   .periods     = 2,
   .stopColor   = LED_COLOR_OFF,
   .numElements = sizeof(specialCanElements_preop_serr_p2)/sizeof(struct stSpecialCodesElements),
   .elements    = specialCanElements_preop_serr_p2,
 }
};


static struct stSpecialCodes specialCanCodes[] =
{
 {
  .single = 0,
  .numElements = sizeof(specialCanPeriods_preop_warn)/sizeof(struct stSpecialPeriod),
  .periods = specialCanPeriods_preop_warn,
 },
 {
  .single = 0,
  .numElements = sizeof(specialCanPeriods_preop_gerr)/sizeof(struct stSpecialPeriod),
  .periods = specialCanPeriods_preop_gerr,
 },
 {
  .single = 0,
  .numElements = sizeof(specialCanPeriods_preop_serr)/sizeof(struct stSpecialPeriod),
  .periods = specialCanPeriods_preop_serr,
 },
 {
   .single = 0,
   .numElements = sizeof(specialCanPeriods_op_warn)/sizeof(struct stSpecialPeriod),
   .periods = specialCanPeriods_op_warn,
  },
  {
   .single = 0,
   .numElements = sizeof(specialCanPeriods_op_gerr)/sizeof(struct stSpecialPeriod),
   .periods = specialCanPeriods_op_gerr,
  },
  {
   .single = 0,
   .numElements = sizeof(specialCanPeriods_op_serr)/sizeof(struct stSpecialPeriod),
   .periods = specialCanPeriods_op_serr,
  },
};

#define MULTI_CAN_MAX (sizeof(specialCanCodes)/sizeof(struct stSpecialCodes))

static tMultiPeriode * CreatePeriod(struct stSpecialPeriod * blinkcode)
{
  tMultiPeriode * ret = NULL;
  tMultiElement * act = NULL;

  DBGFUNC();

  if(blinkcode != NULL)
    {
      size_t   i;
      tTimeMS  timebase = 0;

      ret = MULTI_NewPeriod(blinkcode->periods, blinkcode->stopColor, 0);
      for(i = 0; i < blinkcode->numElements; i++)
      {
        tMultiElement * new = NULL;
        timebase += blinkcode->elements[i].change;
        new = MULTI_NewElement(blinkcode->elements[i].color, timebase);
        if(ret->list == NULL)
        {
          ret->list = new;
          ret->pAct = ret->list;
          act = ret->list;
        }
        else
        {
          act->pNext = new;
          act = act->pNext;
        }
      }
      ret->timeBase = timebase;
    }
  return ret;
}

tTimeMS tbase_ggt(tTimeMS a,tTimeMS b)
{
  if(b == 0)
     return a;
  return tbase_ggt(b, a % b);

}
static void CorrectTimebase(tMultiPeriode * blinkcode)
{
  if(blinkcode->pNext != NULL)
  {
    tMultiPeriode * act = blinkcode;
    tTimeMS         ggt = act->timeBase;

    DBGFUNC();

    while(act != NULL)
    {
      ggt = tbase_ggt(ggt, act->timeBase);
      act = act->pNext;
    }
    act = blinkcode;
    while(act != NULL)
    {
      act->timeBase = ggt;
      act = act->pNext;
    }
  }
}

static tMultiPeriode * CreateCode(struct stSpecialCodes * blinkcode)
{
  tMultiPeriode * ret = NULL;
  tMultiPeriode * act = NULL;

  DBGFUNC();

  if(blinkcode != NULL)
  {
    size_t i;
    for(i = 0 ; i < blinkcode->numElements; i++)
    {
      tMultiPeriode * new = NULL;
      new = CreatePeriod(&blinkcode->periods[i]);
      if(ret == NULL)
      {
        ret = new;
        //ret->pAct = ret->list;
        act = ret;
      }
      else
      {
        act->pNext = new;
        act = act->pNext;
      }
    }
    CorrectTimebase(ret);
    if(blinkcode->single == 0)
    {
      act->pNext = ret;
    }

  }

  return ret;
}
int LEDHANDLER_SetCan(tLedNr ledNr, void * data, void * user_data)
{
  (void) user_data;
  tLedCan * seq = (tLedCan*) data;
  tMultiSequence  sequence;
  tMultiPeriode * period = NULL;
  if(seq == NULL)
  {
    return -1;
  }
  MULTI_InitSequence(&sequence);
  period = CreateCode(&specialCanCodes[seq->canCode]);
  MULTI_AddPeriodToSequence(&sequence,period);
  if(LED_RETURN_OK == SEQUENTIAL_SetBlinkSequence(ledNr,&sequence))
  {
    ledserver_LEDCTRL_SetLedStateData(ledNr,data,sizeof(tLedCan));
    return 0;
  }
  else
  {
    MULTI_DeinitSequence(&sequence);
    return -1;
  }

}

//---- End of source file ------------------------------------------------------
