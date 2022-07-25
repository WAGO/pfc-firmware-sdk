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
#include <dirent.h>
#include "leds.h"

#define SYSFS_PATH "/dev/leds/"
#define BRIGHTNESS_NAME "brightness"
#define TRIGGER_NAME "trigger"
#define OVERWRITE_PIN "over-"
#define BRIGHTNES_VALUE_ON "255"
#define BRIGHTNES_VALUE_OFF "0"
#define BASENAME_SIZE 16

typedef struct stLedGpio{
    int     fd_grn;         //filedescriptor to GPIO of green led
    int     fd_red;         //filedescriptor to GPIO of red led
    int     fd_blue;        //filedescriptor to GPIO of blue led
    uint8_t state;          //actual state of multicolor-led
    uint8_t invalidStates;  //impossible states
    bool    changed;        //indicates that the hardware was changed so, that
                            //we need to read the new state with GetGpio
    char    basename[BASENAME_SIZE];   //basename of led determined by sysfs
}tLedSysFs;

//open only links which ends with allowed colors in sys/class/leds 
typedef struct stLedColorLinkNames{
  char szColorName[16];
}tLedColorLinkNames;

const tLedColorLinkNames allowedColorNames[] = {
  {"-red"},
  {"-green"},
  {"-blue"},
  {"-white"},
  {"-yellow"},
  {"-cyan"},
  {"-magenta"},
  // this line must always be the last one - don't remove it!
  {""}
};

static int numberOfLeds = 0;
static tLedSysFs * leds = NULL;

static void GetBasenameAndColor(const char * name, char * basename, char * colorName)
{
  int i = 0, j=0;
  while(name[i] != 0 && name[i] != '-')
  {
    basename[i]=name[i];
    i++;
  }
  basename[i]=0;
  i++;
  while(name[i] != 0 )
  {
    colorName[j++]=name[i++];
  }
  colorName[j]=0;
}

uint8_t GetOrCreateLedByBasename(const char * basename)
{
  uint8_t led_nr = 0;


  if(leds != NULL)
  {
    for(led_nr=0;led_nr<numberOfLeds;led_nr++)
    {
      if(!strcmp(leds[led_nr].basename, basename))
      {
        break;
      }
    }
  }

  if(led_nr >= numberOfLeds)
  {
    led_nr = numberOfLeds;
    numberOfLeds++;
    leds = realloc(leds,numberOfLeds * sizeof(tLedSysFs));
    strcpy(leds[led_nr].basename,basename);
    leds[led_nr].changed = true;
    leds[led_nr].fd_blue = -1;
    leds[led_nr].fd_grn = -1;
    leds[led_nr].fd_red = -1;
    leds[led_nr].invalidStates = 0xFF;
    leds[led_nr].state = 0;
  }

  return led_nr;
}

static void OpenColor(const char * name)
{
  char basename[BASENAME_SIZE];
  char colorName[256];
  char brightnessPath[1024];
  char triggerPath[1024];
  uint8_t led_nr = -1;
  int fd = -1;
  GetBasenameAndColor(name,basename,colorName);
  //check if LED is an overwrite-pin
  if(!strncmp(colorName,OVERWRITE_PIN,strlen(OVERWRITE_PIN)))
  {
    return;
  }
  led_nr=GetOrCreateLedByBasename(basename);

  //set Trigger to none
  sprintf(triggerPath,"%s/%s/%s",SYSFS_PATH,name,TRIGGER_NAME);
  fd = open(triggerPath, O_WRONLY);
  if(fd >= 0)
  {
    char none[]="none";
    write(fd, none, sizeof(none));
    close(fd);
  }

  sprintf(brightnessPath,"%s/%s/%s",SYSFS_PATH,name,BRIGHTNESS_NAME);
  fd = open(brightnessPath, O_RDWR);

  if(fd >= 0)
  {
    if(!strcmp(colorName,"red"))
    {
      if(leds[led_nr].fd_red >= 0)
      {
        close(leds[led_nr].fd_red);
      }
      leds[led_nr].fd_red = fd;
      leds[led_nr].invalidStates &= ~LED_RED;
    }
    else if(!strcmp(colorName,"green"))
    {
      if(leds[led_nr].fd_grn >= 0)
      {
        close(leds[led_nr].fd_grn);
      }
      leds[led_nr].fd_grn = fd;
      leds[led_nr].invalidStates &= ~LED_GREEN;
    }
    else if(!strcmp(colorName,"blue"))
    {
      if(leds[led_nr].fd_blue >= 0)
      {
        close(leds[led_nr].fd_blue);
      }
      leds[led_nr].fd_blue = fd;
      leds[led_nr].invalidStates &= ~LED_BLUE;
    }
  }
}


/*Initialisieren der LED Steuerung*/
int InitLed( void )
{
  DIR *dp;
  struct dirent *ep;
  int ret = 0;
  uint8_t i;
  char * pPos;
  dp = opendir (SYSFS_PATH);

  if (dp != NULL)
  {
    while ((ep = readdir (dp)))
    {
      if(ep->d_name[0] != '.')
      {
        pPos = strrchr(ep->d_name, '-');
        if (pPos)
        {
          i = 0;
          while (allowedColorNames[i].szColorName[0])
          {     
            //open only links which ends with allowed colors in sys/class/leds      
            if (strcmp(pPos, allowedColorNames[i].szColorName) == 0) 
            {
              //printf("use: %s \n", ep->d_name);
              OpenColor(ep->d_name);
            }
            i++;
          }
        }
      }
    }
    (void) closedir (dp);
  }
  else
  {
    perror ("Couldn't open the sysfs led dir");
  }

  //read ledValues
  for(i=0;i<numberOfLeds;i++)
  {
    (void) GetLed(i);
  }

  return ret;
}

static void LED_on_off(int fd, int on)
{
  if (fd < 0)
    return;
  if (on)
    write(fd, BRIGHTNES_VALUE_ON, sizeof BRIGHTNES_VALUE_ON -1);
  else
    write(fd, BRIGHTNES_VALUE_OFF, sizeof BRIGHTNES_VALUE_OFF -1);
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
    LED_on_off(leds[led_nr].fd_grn, color & LED_GREEN);
    LED_on_off(leds[led_nr].fd_red, color & LED_RED);
    LED_on_off(leds[led_nr].fd_blue, color & LED_BLUE);

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
      char buf[8];
      lseek(leds[led_nr].fd_grn, SEEK_SET, 0);
      read(leds[led_nr].fd_grn, &buf, 3);
      leds[led_nr].state |= buf[0] != '0'? LED_GREEN : 0;
    }

    if(leds[led_nr].fd_red >= 0)
    {
      char buf[8];
      lseek(leds[led_nr].fd_red, SEEK_SET, 0);
      read(leds[led_nr].fd_red, &buf, 3);
      leds[led_nr].state |= buf[0] != '0'? LED_RED : 0;
    }

    if(leds[led_nr].fd_blue >= 0)
    {
      char buf[8];
      lseek(leds[led_nr].fd_blue, SEEK_SET, 0);
      read(leds[led_nr].fd_blue, &buf, 3);
      leds[led_nr].state |= buf[0] != '0'? LED_BLUE : 0;
    }

    leds[led_nr].changed = false;
  }

  return leds[led_nr].state;
}

int    GetNoOfLeds(void)
{
 return numberOfLeds;
}

int     GetLedBaseName(uint8_t led_nr,char *name_buffer)
{
  if(led_nr >= numberOfLeds)
  {
    return -1;
  }
  strcpy(name_buffer, leds[led_nr].basename);
  return 0;
}

