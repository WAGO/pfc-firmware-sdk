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
///  \file     ledmisc.c
///
///  \version  $Rev: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <testapp.h>
#include "ledmisc.h"

tLedReturnNames results[] =
{
 {.code = LED_RETURN_OK, .name="LED_RETURN_OK"},
 {.code = LED_RETURN_ERROR, .name="LED_RETURN_ERROR"},
 {.code = LED_RETURN_ERROR_NO_NAME_AVAILABLE, .name="LED_RETURN_ERROR_NO_NAME_AVAILABLE"},
 {.code = LED_RETURN_ERROR_NUMBER, .name="LED_RETURN_ERROR_NUMBER"},
 {.code = LED_RETURN_ERROR_NAME, .name="LED_RETURN_ERROR_NAME"},
 {.code = LED_RETURN_ERROR_PARAMETER, .name="LED_RETURN_ERROR_PARAMETER"},
 {.code = LED_RETURN_ERROR_STATECLASS, .name="LED_RETURN_ERROR_STATECLASS"},
 {.code = LED_RETURN_ERROR_RESET_LED, .name="LED_RETURN_ERROR_RESET_LED"},
 {.code = LED_RETURN_ERROR_RESET_LED_USER_DATA, .name="LED_RETURN_ERROR_RESET_LED_USER_DATA"},
 {.code = LED_RETURN_ERROR_SET_LED, .name="LED_RETURN_ERROR_SET_LED"},
 {.code = LED_RETURN_ERROR_GET_LED, .name="LED_RETURN_ERROR_GET_LED"},
 {.code = LED_RETURN_ERROR_BUFFERSIZE, .name="LED_RETURN_ERROR_BUFFERSIZE"},
};

const char * GetErrorString(tLedReturnCode code)
{
  size_t i;
  char * ret = NULL;
  for(i=0;i<(sizeof(results)/sizeof(tLedReturnNames));i++)
  {
    if(code == results[i].code)
    {
      ret = results[i].name;
      break;
    }
  }
  return (const char *)ret;
}


tLedColorNames colors[] =
{
 {.color = LED_COLOR_OFF, .name="LED_COLOR_OFF"},
 {.color = LED_COLOR_GREEN, .name="LED_COLOR_GREEN"},
 {.color = LED_COLOR_RED, .name="LED_COLOR_RED"},
 {.color = LED_COLOR_YELLOW, .name="LED_COLOR_YELLOW"},
 {.color = LED_COLOR_BLUE, .name="LED_COLOR_BLUE"},
 {.color = LED_COLOR_CYAN, .name="LED_COLOR_CYAN"},
 {.color = LED_COLOR_MAGENTA, .name="LED_COLOR_MAGENTA"},
 {.color = LED_COLOR_WHITE, .name="LED_COLOR_WHITE"},
};

tColorIter GetColorIter(void)
{
  return 0;
}

int NextColorIter(tColorIter * iter)
{
  if(*iter == (sizeof(colors)/sizeof(tLedColorNames))-1)
  {
    return 0;
  }

  (*iter)++;

  return 1;
}
const char * GetColorName(tColorIter iter)
{
  if(iter >= (tColorIter)(sizeof(colors)/sizeof(tLedColorNames)))
  {
    return NULL;
  }
  return colors[iter].name;
}
const char * GetColorNameByValue(tLedStaticColor val)
{
  size_t i;
  char * ret = NULL;
  for(i=0;i<(sizeof(colors)/sizeof(tLedColorNames));i++)
  {
    if(val == colors[i].color)
    {
      ret = colors[i].name;
      break;
    }
  }
  return (const char *)ret;
}
tLedStaticColor GetColorVal(tColorIter iter)
{
  if(iter >= (tColorIter)(sizeof(colors)/sizeof(tLedColorNames)))
  {
    return -1;
  }
  return colors[iter].color;
}

tLedStateNames classes[] =
{
 {.class = LED_STATE_OFF, .name="LED_STATE_OFF"},
 {.class = LED_STATE_STATIC, .name="LED_STATE_STATIC"},
 {.class = LED_STATE_BLINK, .name="LED_STATE_BLINK"},
 {.class = LED_STATE_750_ERR, .name="LED_STATE_750_ERR"},
 {.class = LED_STATE_CAN, .name="LED_STATE_CAN"},
 {.class = LED_STATE_ROCKET_ERR2, .name="LED_STATE_ROCKET_ERR2"},
 {.class = LED_STATE_ROCKET_ERR3, .name="LED_STATE_ROCKET_ERR3"},
};

const char * GetStateString(tLedStateClass class)
{
  size_t i;
  char * ret = NULL;
  for(i=0;i<(sizeof(classes)/sizeof(tLedStateNames));i++)
  {
    if(class == classes[i].class)
    {
      ret = classes[i].name;
      break;
    }
  }
  return (const char *)ret;
}

//---- End of source file ------------------------------------------------------
