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
///  \file     ledmisc.c
///
///  \version  $Rev: 13867 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <testapp.h>
#include <unistd.h>
#include "ledmisc.h"

#include "diagnostic_xml.h"
#include <diagnostic/diagnostic_API.h>

static  tLedBehavior    ledBehavior;

#define LEDCONFIG_XML_DOCUMENT          "/tmp/led.xml"
#define DIAG_XML_FILE "/etc/specific/diagnostic.xml"

static int XmlLedValid = 1;

#define MAX_ID 65535

int max_id=0;

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


static const char * ColorToString(tLedStaticColor color)
{
  const char * ret = NULL;

  switch(color)
  {
    case LED_COLOR_RED:
      ret = "RED";
      break;
    case LED_COLOR_GREEN:
      ret = "GRN";
      break;
    case LED_COLOR_YELLOW:
      ret = "YLW";
      break;
    case LED_COLOR_BLUE:
      ret = "BLU";
      break;
    case LED_COLOR_CYAN:
      ret = "CYN";
      break;
    case LED_COLOR_MAGENTA:
      ret = "MAG";
      break;
    case LED_COLOR_WHITE:
      ret = "WHT";
      break;
    case LED_COLOR_OFF:
      ret = "OFF";
      break;
  }
  return ret;
}

static void StateToString(char * statestring, tLedStateClass state, tLedVars * ledVars)
{
     switch(state)
     {
       case LED_STATE_STATIC:
         sprintf(statestring,"STATIC_%s",
                 ColorToString(ledVars->staticVar));
         break;
       case LED_STATE_BLINK:
         if(ledVars->blinkVar.color2 == LED_COLOR_OFF)
         {
           sprintf(statestring,"BLINK_%s",
                   ColorToString(ledVars->blinkVar.color1));
         }
         else
         {
           sprintf(statestring,"BLINK_%s_%s",
                   ColorToString(ledVars->blinkVar.color1),
                   ColorToString(ledVars->blinkVar.color2));
         }
        break;
       case LED_STATE_FLASH:
         sprintf(statestring,"FLASH_%s_%s",
                 ColorToString(ledVars->flashVar.flashColor),
                 ColorToString(ledVars->flashVar.staticColor));
         break;
       case LED_STATE_750_ERR:
         sprintf(statestring,"ERR_SEQ_750");
         break;
       case LED_STATE_CAN:
         sprintf(statestring,"SPECIAL_BLINK");
         break;
       default:
         break;
     }

}

static void NewLed(FILE * fp,const char * name, tLedStateClass state, tLedVars * ledVars)
{
  char stateString[64];

  StateToString(stateString,state,ledVars);

  fprintf(fp, "    <led name=\"%s\" state=\"%s\">\n",name,stateString);
  switch(state)
  {
    case LED_STATE_750_ERR:
      fprintf(fp, "      <blinkcode>%d</blinkcode>\n",ledVars->err750Var.errorCode);
      fprintf(fp, "      <blinkarg>%d</blinkarg>\n",ledVars->err750Var.errorArg);
      break;
    case LED_STATE_FLASH:
      fprintf(fp, "      <on>%d</on>\n",ledVars->flashVar.flashTime);
    break;
    case LED_STATE_BLINK:
      fprintf(fp, "      <on>%d</on>\n",ledVars->blinkVar.time1);
      fprintf(fp, "      <off>%d</off>\n",ledVars->blinkVar.time2);
    break;
    case LED_STATE_STATIC:
    default:
      //nothing to do;
      break;
  }
    /*<on>300</on>
    <off>300</off>*/
  fprintf(fp, "    </led>\n");
}

static void NewEvent(FILE * fp,int idIter, const char * name, tLedStateClass state, tLedVars * ledVars)
{
  fprintf(fp, "  <event id=\"x%.4X\" name=\"testId%d\" persistent=\"none\">\n", idIter,idIter);
  NewLed(fp, name,state,ledVars);
  fprintf(fp, "    <string>\n      <en>TestId %d</en>\n    </string>\n",idIter);
  fprintf(fp, "  </event>\n");
}

int ChkColor(tLedStaticColor color)
{
  if(   (color == LED_COLOR_BLUE)
     || (color == LED_COLOR_CYAN)
     || (color == LED_COLOR_MAGENTA)
     || (color == LED_COLOR_WHITE))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int CheckStatic(tLedVars * ledVars)
{
  return ChkColor((tLedStaticColor)ledVars->staticVar);
}

int GenerateStaticId(FILE * fp,int idIter ,tLedBehavior * ledBehavior)
{
  tColorIter colorIter;
  tLedNames    *  actName = ledBehavior->names;
  tLedVars        ledVars;
  //static
  while(actName != NULL)
  {
    colorIter = GetColorIter();
    do{
      ledVars.staticVar = (tLedStatic)GetColorVal(colorIter);
      if(XmlLedValid == CheckStatic(&ledVars))
      {
        NewEvent(fp,idIter, actName->ledName, LED_STATE_STATIC, &ledVars);
        if((idIter++) >= MAX_ID)
        {
          puts("Fehler: die IDs gehen aus!");
          return 0;
        }
      }
    }while(NextColorIter(&colorIter));
    actName = actName->pNext;
  }
  return idIter;
}

int CheckBlink(tLedVars * ledVars)
{
  if(ledVars->blinkVar.color1 == ledVars->blinkVar.color2)
  {
    return 0;
  }
  if(!ChkColor(ledVars->blinkVar.color1))
  {
    return 0;
  }
  if(!ChkColor(ledVars->blinkVar.color2))
  {
    return 0;
  }
  return 1;
}

int GenerateBlinkId(FILE * fp,int idIter ,tLedBehavior * ledBehavior)
{
  tColorIter color1Iter;
  tColorIter color2Iter;
  tLedNames    *  actName = ledBehavior->names;
  tLedVars        ledVars;
  //static
  while(actName != NULL)
  {
    color2Iter = GetColorIter();
    do{
      color1Iter = GetColorIter();
      do{
        ledVars.blinkVar.color1 = GetColorVal(color1Iter);
        ledVars.blinkVar.color2 = GetColorVal(color2Iter);
        ledVars.blinkVar.time1 = 300;
        ledVars.blinkVar.time2 = 300;
        if(XmlLedValid == CheckBlink(&ledVars))
        {
          NewEvent(fp,idIter, actName->ledName, LED_STATE_BLINK, &ledVars);
          if((idIter++) >= MAX_ID)
          {
            puts("Fehler: die IDs gehen aus!");
            return 0;
          }
        }
      }while(NextColorIter(&color1Iter));
    }while(NextColorIter(&color2Iter));
    actName = actName->pNext;
  }
  return idIter;
}

int CheckFlash(tLedVars * ledVars)
{
  if(ledVars->flashVar.flashColor == ledVars->flashVar.staticColor)
  {
    return 0;
  }
  if(!ChkColor(ledVars->flashVar.flashColor))
  {
    return 0;
  }
  if(!ChkColor(ledVars->flashVar.staticColor))
  {
    return 0;
  }
  return 1;
}

int GenerateFlashId(FILE * fp,int idIter ,tLedBehavior * ledBehavior)
{
  tColorIter color1Iter;
  tColorIter color2Iter;
  tLedNames    *  actName = ledBehavior->names;
  tLedVars        ledVars;
  //static
  while(actName != NULL)
  {
    color2Iter = GetColorIter();
    do{
      color1Iter = GetColorIter();
      do{
        ledVars.flashVar.flashColor = GetColorVal(color1Iter);
        ledVars.flashVar.staticColor = GetColorVal(color2Iter);
        ledVars.flashVar.flashTime = 500;
        if(XmlLedValid == CheckFlash(&ledVars))
        {
          NewEvent(fp,idIter, actName->ledName, LED_STATE_FLASH, &ledVars);
          if((idIter++) >= MAX_ID)
          {
            puts("Fehler: die IDs gehen aus!");
            return 0;
          }
        }
      }while(NextColorIter(&color1Iter));
    }while(NextColorIter(&color2Iter));
    actName = actName->pNext;
  }
  return idIter;
}

int GenerateErr750Id(FILE * fp,int idIter ,tLedBehavior * ledBehavior)
{
  tLedNames    *  actName = ledBehavior->names;
  tLedVars        ledVars;
  //static
  while(actName != NULL)
  {
    ledVars.err750Var.errorCode = 2;
    ledVars.err750Var.errorArg = 3;
    NewEvent(fp,idIter, actName->ledName, LED_STATE_750_ERR, &ledVars);
    if((idIter++) >= MAX_ID)
    {
      puts("Fehler: die IDs gehen aus!");
      return 0;
    }
    actName = actName->pNext;
  }
  return idIter;
}

int GenerateCanId(FILE * fp,int idIter ,tLedBehavior * ledBehavior)
{
  tLedNames    *  actName = ledBehavior->names;
  tLedVars        ledVars;
  //static
  while(actName != NULL)
  {
    int i;
    for(i=0;i<6;i++)
    {
      ledVars.canVar.canCode = i;
      NewEvent(fp,idIter, actName->ledName, LED_STATE_CAN, &ledVars);
      if((idIter++) >= MAX_ID)
      {
        puts("Fehler: die IDs gehen aus!");
        return 0;
      }
    }
    actName = actName->pNext;
  }
  return idIter;
}

void getDeviceLEDs(void)
{
  static int once = 1;

  if(once)
  {
    ledBehavior.defaults = NULL;
    ledBehavior.names = NULL;
    ParseLedDoc(LEDCONFIG_XML_DOCUMENT, &ledBehavior);
    once = 0;
  }
}

//generiere ein XML file mit allen möglichen Blinkcodes für alle vorhandenen LEDs
int GenerateValidOnlyXmlFile(void)
{
  FILE * fp;


  getDeviceLEDs();



  printf("generiere XML-Datei \"%s\"\n",DIAG_XML_FILE);
  fp = fopen(DIAG_XML_FILE,"w");
  if(fp == NULL)
  {
    perror("Fehler beim öffnen");
  }
  else
  {
    int i=0;
    XmlLedValid = 1;
    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<diagnostic>\n");
    fprintf(fp, "<eventclass class_range=\"x0000\" name=\"ledtest\">\n");
    //static
    i=GenerateStaticId(fp,i,&ledBehavior);
    //blink
    i=GenerateBlinkId(fp,i,&ledBehavior);
    //flash
    i=GenerateFlashId(fp,i,&ledBehavior);
    //error 750
    i=GenerateErr750Id(fp,i,&ledBehavior);
    //CAN
    i=GenerateCanId(fp,i,&ledBehavior);

    fprintf(fp, "</eventclass>");
    fprintf(fp, "</diagnostic>\n");
    fflush(fp);
    fflush(fp);
    fclose(fp);
    max_id =i;
  }
  return 1;
}


int GenerateInvalidOnlyXmlFile(void)
{
  FILE * fp;


  getDeviceLEDs();



   printf("generiere XML-Datei \"%s\"\n",DIAG_XML_FILE);
  fp = fopen(DIAG_XML_FILE,"w");
  if(fp == NULL)
  {
    perror("Fehler beim öffnen");
  }
  else
  {
    int i=0;
    XmlLedValid =0;
    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<diagnostic>\n");
    fprintf(fp, "<eventclass class_range=\"x0000\" name=\"ledtest\">\n");
    //static
    i=GenerateStaticId(fp,i,&ledBehavior);
    //blink
    i=GenerateBlinkId(fp,i,&ledBehavior);
    //flash
    i=GenerateFlashId(fp,i,&ledBehavior);
    //error 750
    i=GenerateErr750Id(fp,i,&ledBehavior);
    //CAN
    i=GenerateCanId(fp,i,&ledBehavior);

    fprintf(fp, "</eventclass>");
    fprintf(fp, "</diagnostic>\n");
    fflush(fp);
    fflush(fp);
    fclose(fp);
    max_id =i;
  }
  return 1;
}

int GenerateXmlFile(void)
{
  FILE * fp;


  getDeviceLEDs();



   printf("generiere XML-Datei \"%s\"\n",DIAG_XML_FILE);
  fp = fopen(DIAG_XML_FILE,"w");
  if(fp == NULL)
  {
    perror("Fehler beim öffnen");
  }
  else
  {
    int i=0;

    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<diagnostic>\n");
    fprintf(fp, "<eventclass class_range=\"x0000\" name=\"ledtest\">\n");
    //static
    XmlLedValid =0;
    i=GenerateStaticId(fp,i,&ledBehavior);
    XmlLedValid =1;
    i=GenerateStaticId(fp,i,&ledBehavior);
    //blink
    i=GenerateBlinkId(fp,i,&ledBehavior);
    XmlLedValid =0;
    i=GenerateBlinkId(fp,i,&ledBehavior);
    //flash
    i=GenerateFlashId(fp,i,&ledBehavior);
    XmlLedValid =1;
    i=GenerateFlashId(fp,i,&ledBehavior);
    //error 750
    i=GenerateErr750Id(fp,i,&ledBehavior);
    XmlLedValid =0;
    i=GenerateErr750Id(fp,i,&ledBehavior);
    //CAN
    i=GenerateCanId(fp,i,&ledBehavior);
    XmlLedValid =1;
    i=GenerateCanId(fp,i,&ledBehavior);

    fprintf(fp, "</eventclass>");
    fprintf(fp, "</diagnostic>\n");
    fflush(fp);
    fflush(fp);
    fclose(fp);
    max_id =i;
  }
  return 1;
}


int GenerateSpecificXmlFile(int valid, tLedStateClass state)
{
  FILE * fp;


  getDeviceLEDs();



   printf("generiere XML-Datei \"%s\"\n",DIAG_XML_FILE);
  fp = fopen(DIAG_XML_FILE,"w");
  if(fp == NULL)
  {
    perror("Fehler beim öffnen");
  }
  else
  {
    int i=0;

    XmlLedValid = valid;

    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<diagnostic>\n");
    fprintf(fp, "<eventclass class_range=\"x0000\" name=\"ledtest\">\n");
    switch(state)
    {
      case LED_STATE_STATIC:
        //static
        i=GenerateStaticId(fp,i,&ledBehavior);
        break;
      case LED_STATE_BLINK:
        //blink
        i=GenerateBlinkId(fp,i,&ledBehavior);
        break;
      case LED_STATE_FLASH:
        //flash
        i=GenerateFlashId(fp,i,&ledBehavior);
        break;
      case LED_STATE_750_ERR:
        //error 750
        i=GenerateErr750Id(fp,i,&ledBehavior);
        break;
      case LED_STATE_CAN:
        //CAN
        i=GenerateCanId(fp,i,&ledBehavior);
        break;
      default:
        break;
    }

    fprintf(fp, "</eventclass>");
    fprintf(fp, "</diagnostic>\n");
    fflush(fp);
    fflush(fp);
    fclose(fp);
    max_id =i;
  }
  return 1;
}

int CheckLedServer(void)
{
  if(system("pidof ledserverd &> /dev/null"))
  {
    //really?
    if(system("pidof ledserverd &> /dev/null"))
      {
        return 0;
      }
      else
      {
        return 1;
      }
  }
  else
  {
    return 1;
  }
}

static void SendAsId(unsigned int id,bool set)
{
  log_tEventId send = 0x40000000 | id;
  log_EVENT_LogId(send,set);
}

int sendAllIds(int times)
{
  int i,j,runs;
  for(runs=times;runs>0;runs++)
  {
    for(i=0;i<=max_id;i++)
    {
      for(j=0;j<=max_id;j++)
      {
        SendAsId((unsigned int)i,true);
        usleep(300000);
        SendAsId((unsigned int)j,true);
        usleep(300000);
        //sleep(1);
        if(!CheckLedServer())
        {
          printf("Fehler: LED-Server abgestürzt ID: %d / %d\n",i,j);
          goto out;
        }
      }
    }
  }
  return 1;
  out:
  return 0;
}

//---- End of source file ------------------------------------------------------
