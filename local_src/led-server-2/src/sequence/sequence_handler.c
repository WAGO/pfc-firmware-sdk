//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <ledserver_API.h>
#include "sequence_handler.h"

typedef struct {
    tLedStateClass     class;
    tLedSchedHandler * callbacks;
}tSequenceHandler;

int LEDHANDLER_SetStatic(tLedNr, void*,void*);
//int LEDHANDLER_ResetStatic(tLedNr, void*);
//int LEDHANDLER_GetStatic(tLedNr, void*);
//int LEDHANDLER_CleanupStatic(void*);

static tLedSchedHandler ledStatic =
{
 .setLedHandler = LEDHANDLER_SetStatic,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

int LEDHANDLER_SetBlink(tLedNr, void*,void*);

static tLedSchedHandler ledBlink =
{
 .setLedHandler = LEDHANDLER_SetBlink,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

int LEDHANDLER_SetErr750(tLedNr, void*,void*);

static tLedSchedHandler ledErr750 =
{
 .setLedHandler = LEDHANDLER_SetErr750,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

int LEDHANDLER_SetFlash(tLedNr, void*,void*);

static tLedSchedHandler ledFlash =
{
 .setLedHandler = LEDHANDLER_SetFlash,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

int LEDHANDLER_SetCan(tLedNr, void*,void*);

static tLedSchedHandler ledCan =
{
 .setLedHandler = LEDHANDLER_SetCan,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

int LEDHANDLER_SetRocketErr2(tLedNr, void*,void*);

static tLedSchedHandler ledRocketErr2 =
{
 .setLedHandler = LEDHANDLER_SetRocketErr2,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

int LEDHANDLER_SetRocketErr3(tLedNr, void*,void*);

static tLedSchedHandler ledRocketErr3 =
{
 .setLedHandler = LEDHANDLER_SetRocketErr3,
 //.cleanupHandler = NULL,
 .getLedHandler = NULL,
 .resetLedHandler =NULL,
 .resetUserDataHandler = NULL
};

static tSequenceHandler callbacks[] = {
 { .class = LED_STATE_STATIC, .callbacks = &ledStatic },
 { .class = LED_STATE_BLINK, .callbacks = &ledBlink },
 { .class = LED_STATE_750_ERR, .callbacks = &ledErr750 },
 { .class = LED_STATE_FLASH, .callbacks = &ledFlash },
 { .class = LED_STATE_CAN, .callbacks = &ledCan },
 { .class = LED_STATE_ROCKET_ERR2, .callbacks = &ledRocketErr2 },
 { .class = LED_STATE_ROCKET_ERR3, .callbacks = &ledRocketErr3 }
};

tLedReturnCode SEQHANDLER_GetSequenceHandler(tLedStateClass status, tLedSchedHandler ** handler)
{
  int i;
  tLedReturnCode ret = LED_RETURN_ERROR_STATECLASS;
  if(handler == NULL)
  {
    return LED_RETURN_ERROR_PARAMETER;
  }

  for(i=0;i<(int)(sizeof(callbacks)/sizeof(tSequenceHandler));i++)
  {
    if(status == callbacks[i].class)
    {
      ret = LED_RETURN_OK;
      *handler = callbacks[i].callbacks;
      break;
    }
  }

  return ret;
}

tLedReturnCode ledserver_LEDCTRL_SetResetUserDataHandler(tLedStateClass status,tLedHandler resetUserDataHandler)
{
  tLedSchedHandler * handler = NULL;
  tLedReturnCode ret =  SEQHANDLER_GetSequenceHandler(status,&handler);
  if(ret == LED_RETURN_OK)
  {
    handler->resetUserDataHandler = resetUserDataHandler;
  }

  return ret;
}
