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
///  \file     interactive.c
///
///  \version  $Rev: 46819 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "testapp.h"
#include "ledmisc.h"
#include <ledserver_API.h>

#include <unistd.h>



static int _Check_GetLedCount()
{
  tLedNr ledNr = ledserver_LEDCTRL_GetLedCount();
  printf("Es wurden %d LEDs erkannt ist das korrekt?\n",ledNr);
  return YesNo();
}

static int _Check_GetLedName()
{
  tLedNr ledNr = ledserver_LEDCTRL_GetLedCount();
  tLedNr i;
  printf("Leds haben folgende Namen:\n");
  for(i=0;i<ledNr;i++)
  {
    char buffer[255];
    ledserver_LEDCTRL_GetLedName(i, buffer,sizeof(buffer));
    printf("led %.2d: %s\n",i,buffer);
  }
  printf("Ist das korrekt");
  return YesNo();
}

static int _allLedsOn()
{
  puts("set all LEDs on");
  tLedNr ledNr = ledserver_LEDCTRL_GetLedCount();
  tLedNr i;
  tColorIter colI = GetColorIter();
  do{
    int ret;
    printf("Alle leds in der Farbe %s einschalten\n", GetColorName(colI));
    for(i=0;i < ledNr;i++)
    {
      tLedStatic set = (tLedStatic)GetColorVal(colI);
      tLedStateClass state;
      tLedReturnCode result;
      result = ledserver_LEDCTRL_SetLed(i, LED_STATE_STATIC, &set,NULL);
      if(result != LED_RETURN_OK)
      {
        char fail[1024];
        sprintf(fail,"Fehler: set LED %d, %s, %s", i,GetColorName(colI),GetErrorString(result));
        TestLogger((const char *)fail);
      }
      result = ledserver_LEDCTRL_GetLed(i, &state, &set,sizeof(set),NULL);
      if(result != LED_RETURN_OK)
      {
        char fail[1024];
        sprintf(fail,"Fehler: get LED %d, %s", i,GetErrorString(result));
        TestLogger((const char *)fail);
      }
      if(state != LED_STATE_STATIC)
      {
        char fail[1024];
        sprintf(fail,"Fehler: get LED %d, got %s but expected %s", i,GetStateString(state),GetStateString(LED_STATE_STATIC));
        TestLogger((const char *)fail);
      }
      if(set != GetColorVal(colI))
      {
        char fail[1024];
        sprintf(fail,"Fehler: get LED %d, got %s but expected %s", i,GetColorNameByValue(set),GetColorName(colI));
        TestLogger((const char *)fail);
      }
    }
    printf("Ist das Ergebnis für %s OK? ", GetColorName(colI));
    ret = YesNo();
    printf("Testergebis: %d\n",ret);
  }while(NextColorIter(&colI));

  return 1;
}


static int _blinkSync()
{
  puts("let all LEDs blink sync");
  tLedNr ledNr = ledserver_LEDCTRL_GetLedCount();
  tLedNr i;


    printf("Alle leds in der Farbe %s blinken lassen\n", GetColorNameByValue(LED_COLOR_GREEN));
    for(i=0;i < ledNr;i++)
    {
      tLedBlink set;
      tLedStateClass state;
      tLedReturnCode result;

      set.color1 = LED_COLOR_GREEN;
      set.time1 = 500;
      set.color2 = LED_COLOR_OFF;
      set.time2 = 500;

      result = ledserver_LEDCTRL_SetLed(i, LED_STATE_BLINK, &set,NULL);
      if(result != LED_RETURN_OK)
      {
        char fail[1024];
        sprintf(fail,"Fehler: set LED %d, %s, %s", i,GetStateString(LED_STATE_BLINK),GetErrorString(result));
        TestLogger((const char *)fail);
      }
      result = ledserver_LEDCTRL_GetLed(i, &state, &set,sizeof(set),NULL);
      if(result != LED_RETURN_OK)
      {
        char fail[1024];
        sprintf(fail,"Fehler: get LED %d, %s", i,GetErrorString(result));
        TestLogger((const char *)fail);
      }
      if(state != LED_STATE_BLINK)
      {
        char fail[1024];
        sprintf(fail,"Fehler: get LED %d, got %s but expected %s", i,GetStateString(state),GetStateString(LED_STATE_BLINK));
        TestLogger((const char *)fail);
      }
     /* if(set.color1)
      {
        char fail[1024];
        sprintf(fail,"Fehler: get LED %d, got %s but expected %s", i,GetColorNameByValue(set),GetColorName(colI));
        TestLogger((const char *)fail);
      }*/
      usleep(100000);
    }
    printf("Blinken die LEDs syncron?");
    return YesNo();

}

#ifdef PFC200_ADV
#define STATE_LED "ec"
#else
#define STATE_LED "io"
#endif

static int _pfc_seq()
{
  tLed750 tl750 = { 2, 3 };
  printf("Zeige den Fehler[%d] mit Argument[%d] auf der PFC200 '" STATE_LED "' LED\n",
         tl750.errorCode, tl750.errorArg);
  ledserver_LEDCTRL_SetLedByName(STATE_LED, LED_STATE_750_ERR, &tl750, NULL);
  sleep(8);
  puts("Entspricht das Blinkmuster den Erwartungen?");
  return YesNo();
}

static int _blinkSwitch()
{
  puts("Eine LED zwischen Static und blink hin und her switchen lassen");
  tLedNr i=0;

  unsigned int sleepTime;
  unsigned int sleepInterval;

  for(sleepTime = 1000000; sleepTime > 500; sleepTime-=100)
  {
	  for(sleepInterval=0;sleepInterval<100;sleepInterval++)
	  {
		  tLedReturnCode result;
		  tLedBlink setBlink;
		  tLedStaticColor setStatic;
		  setBlink.color1 = LED_COLOR_OFF;
		  setBlink.time1 = 50;
		  setBlink.color2 = LED_COLOR_RED;
		  setBlink.time2 = 10;

		  result = ledserver_LEDCTRL_SetLed(0, LED_STATE_BLINK, &setBlink,NULL);
		  if(result != LED_RETURN_OK)
		  {
		    char fail[1024];
		    sprintf(fail,"Fehler: setBlink LED %d, %s, %s", i,GetStateString(LED_STATE_BLINK),GetErrorString(result));
		    TestLogger((const char *)fail);
		  }
		  usleep(sleepTime);
		  setStatic = LED_COLOR_YELLOW;
		  result = ledserver_LEDCTRL_SetLed(0, LED_STATE_STATIC, &setStatic,NULL);
		  usleep(85000);
	  }
  }
  tLedBlink set;

  set.color1 = LED_COLOR_GREEN;
  set.time1 = 500;
  set.color2 = LED_COLOR_OFF;
  set.time2 = 500;

  ledserver_LEDCTRL_SetLed(i, LED_STATE_BLINK, &set,NULL);

  printf("Test erfolgreich abgeschlossen");
  return YesNo();
}

tTestCase interactiveTests[] =
{
 { .name="chk_count", .help="prüfen ob alle LEDs erkannt wurden" ,  .function=_Check_GetLedCount},
 { .name="chk_names", .help="prüfen ob die LED namen korrekt sind" ,  .function=_Check_GetLedName},
 { .name="allLedsOn", .help="alle LEDs gleichzeitig einschalten, ale farben" ,  .function=_allLedsOn},
 { .name="blinkSync", .help="alle LEDs sollen synchron blinken" ,  .function=_blinkSync},
 { .name="blinkSwitch", .help="eine LED zwischen static und blink hin und her schalten" ,  .function=_blinkSwitch},
 { .name="pfc_seq",   .help="Einen PFC Fehler über die " STATE_LED " LED ausgeben. (Code 2, Argument 3)" ,  .function=_pfc_seq},
 { .name="exit",        .help="menü verlassen" ,                      .function=TestExit},
};

size_t numberOfInteractiveTests = sizeof(interactiveTests)/sizeof(tTestCase);



//---- End of source file ------------------------------------------------------
