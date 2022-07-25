//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

/*
 * leds.c
 */

//diese Datei wird sich für den PAC ändern

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include "leds.h"
#include "config.h"

#define GPIO_INVALID        -1
//#define PFC200_EVALBOARD 0
//#define PFC200 1
//#define PERSEPCTO 1
#if PFC200_EVALBOARD
#include "pfc200_eval_gpios.h"
#elif PFC200
#include "pfc200_gpios.h"
#elif PERSPECTO
#include "perspecto_gpios.h"
#endif


#define EXPORT_PATH       "/sys/class/gpio/export"
#define ACTIVE_LOW_FORMAT "/sys/class/gpio/gpio%d/active_low"
#define ACTIVE_LOW_VALUE  "0"
#define DIRECTION_FORMAT  "/sys/class/gpio/gpio%d/direction"
#define DIRECTION_VALUE   "low"
#define VALUE_FORMAT      "/sys/class/gpio/gpio%d/value"

typedef struct stLedGpio{
    int     fd_grn;         //filedescriptor to GPIO of green led
    int     fd_red;         //filedescriptor to GPIO of red led
    int     fd_blue;         //filedescriptor to GPIO of blue led
    uint8_t state;          //actual state of multicolor-led
    uint8_t invalidStates;  //impossible states
    bool    changed;        //indicates that the hardware was changed so, that
                            //we need to read the new state with GetGpio
}tLedGpio;

struct stInitLeds {
    int     grn;
    int     red;
    int     blue;
};

/*
 *    LED Position on Hardware:
 *             00|01
 *             --+--
 *             03|04
 *             --+--
 *             05|06
 *             --+--
 *             07|08
 *             --+--
 *             09|10
 *             --+--
 *             11|12
 */

static const struct stInitLeds ledInitials[] = {
    {GPIO_BF_GRN,        GPIO_BF_RED,        GPIO_BF_BLUE},
    {GPIO_SYS_GRN,       GPIO_SYS_RED,       GPIO_SYS_BLUE},
    {GPIO_DIA_GRN,       GPIO_DIA_RED,       GPIO_DIA_BLUE},
    {GPIO_RUN_GRN,       GPIO_RUN_RED,       GPIO_RUN_BLUE},
    {GPIO_U4_GRN,        GPIO_U4_RED,        GPIO_U4_BLUE},
    {GPIO_IO_GRN,        GPIO_IO_RED,        GPIO_IO_BLUE},
    {GPIO_U3_GRN,        GPIO_U3_RED,        GPIO_U3_BLUE},
    {GPIO_FB1_GRN,       GPIO_FB1_RED,       GPIO_FB1_BLUE},//MS
    {GPIO_U2_GRN,        GPIO_U2_RED,        GPIO_U2_BLUE},
    {GPIO_FB2_GRN,       GPIO_FB2_RED,       GPIO_FB2_BLUE},//NS
    {GPIO_U1_GRN,        GPIO_U1_RED,        GPIO_U1_BLUE},
    {GPIO_CAN_GRN,       GPIO_CAN_RED,       GPIO_CAN_BLUE},
};
static const int numberOfLeds = sizeof(ledInitials) / sizeof(struct stInitLeds);
static tLedGpio leds[sizeof(ledInitials) / sizeof(struct stInitLeds)];

int OpenGpio(int gpio_num)
{
 // int written_bytes;
  int fd;
  char path[1024];
  if(gpio_num == GPIO_INVALID)
  {
    return -1;
  }

  sprintf(path, VALUE_FORMAT, gpio_num);
  fd = open(path, O_RDWR);
  if(fd < 0)
  {
    fprintf(stderr,"GPIO %d open failed. Reason: %s\n", gpio_num, strerror(errno));
  }

  return fd;
}

/*Initialisieren der LED Steurung*/
int InitLed( void )
{
  int i, ret = 0;

  for(i = 0; i < numberOfLeds; i++)
  {
    leds[i].invalidStates = 0;
    leds[i].fd_grn = OpenGpio(ledInitials[i].grn);
    if(   (leds[i].fd_grn <= 0))
    {
      if(ledInitials[i].grn != GPIO_INVALID)
      {
        ret = -1;
      }
      leds[i].invalidStates |= LED_GREEN;
    }

    leds[i].fd_red = OpenGpio(ledInitials[i].red);
    if(   (leds[i].fd_red <= 0))
    {
      if(ledInitials[i].red != GPIO_INVALID)
      {
        ret = -1;
      }
      leds[i].invalidStates |= LED_RED;
    }

    leds[i].fd_blue = OpenGpio(ledInitials[i].blue);
    if(   (leds[i].fd_blue <= 0))
    {
      if(ledInitials[i].blue != GPIO_INVALID)
      {
        ret = -1;
      }
      leds[i].invalidStates |= LED_BLUE;
    }
    leds[i].state = LED_OFF;
    leds[i].changed = true;
  }

   return ret;
}

/*LED Einschalten*/
int8_t SetLed(uint8_t led_nr,
                uint8_t color)
{
  //return error if led cannot beset correctly
  if(leds[led_nr].invalidStates & color)
  {
    return -1;
  }

  //check if led has already the wanted color
  if(leds[led_nr].state != color)
  {
    //set Green LED
    if(leds[led_nr].fd_grn >= 0)
    {
      char buf = color & LED_GREEN ? '1' : '0';
      write(leds[led_nr].fd_grn, &buf, 1);
    }
    //set Red LED
    if(leds[led_nr].fd_red >= 0)
    {
      char buf = color & LED_RED ? '1' : '0';
      write(leds[led_nr].fd_red, &buf, 1);
    }
    //set Blue LED
    if(leds[led_nr].fd_blue >= 0)
    {
      char buf = color & LED_BLUE ? '1' : '0';
      write(leds[led_nr].fd_blue, &buf, 1);
    }
    //set status flags
    leds[led_nr].changed = true;
    leds[led_nr].state   = color;
  }
  return 0;
}

/*LED Ausschalten*/
int8_t DelLed(uint8_t led_nr)
{
  return SetLed(led_nr,LED_OFF);
}

/*LED Abfragen*/
uint8_t GetLed(uint8_t led_nr)
{
  if(leds[led_nr].changed == true)
  {
    leds[led_nr].state  = 0;

    if(leds[led_nr].fd_grn >= 0)
    {
      char buf;
      lseek(leds[led_nr].fd_grn, SEEK_SET, 0);
      read(leds[led_nr].fd_grn, &buf, 1);
      leds[led_nr].state |= buf == '1'? LED_GREEN : 0;
    }

    if(leds[led_nr].fd_red >= 0)
    {
      char buf;
      lseek(leds[led_nr].fd_red, SEEK_SET, 0);
      read(leds[led_nr].fd_red, &buf, 1);
      leds[led_nr].state |= buf == '1'? LED_RED : 0;
    }

    if(leds[led_nr].fd_blue >= 0)
    {
      char buf;
      lseek(leds[led_nr].fd_blue, SEEK_SET, 0);
      read(leds[led_nr].fd_blue, &buf, 1);
      leds[led_nr].state |= buf == '1'? LED_BLUE : 0;
    }

    leds[led_nr].changed = false;
  }

  return leds[led_nr].state;
}

int    GetNoOfLeds(void)
{
 return numberOfLeds;
}

int GetLedBaseName(uint8_t led_nr,char *name_buffer)
{
  if(name_buffer == NULL)
  {
    return -1;
  }
  sprintf(name_buffer,"GPIO");
  return 0;
}
