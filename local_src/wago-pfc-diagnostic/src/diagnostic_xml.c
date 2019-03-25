/*
 * diagnostig_xml.c
 *
 *TODO: make a real Library including code Style Rules!!!
 *
 *  Created on: 28.05.2012
 *      Author: flormel
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "diagnostic_xml.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>



const char diagnostic_xml_default_lan[] = "en";
tLedFiles * pActualFile = NULL;

#define DIAG_ID_SAVE_PERS 0x80000000
#define DIAG_ID_SAVE_NO   0x40000000

typedef struct {
    tLedStaticColor color;
    char            string[16];
}tColorString;

tColorString ledColors[] = {
{.color = LED_COLOR_OFF,      .string = "off"},
{.color = LED_COLOR_OFF,      .string = "OFF"},
{.color = LED_COLOR_RED,      .string = "red"},
{.color = LED_COLOR_RED,      .string = "RED"},
{.color = LED_COLOR_GREEN,    .string = "green"},
{.color = LED_COLOR_GREEN,    .string = "GRN"},
{.color = LED_COLOR_YELLOW,   .string = "yellow"},
{.color = LED_COLOR_YELLOW,   .string = "orange"},
{.color = LED_COLOR_YELLOW,   .string = "YLW"},
{.color = LED_COLOR_BLUE,     .string = "blue"},
{.color = LED_COLOR_BLUE,     .string = "BLU"},
{.color = LED_COLOR_CYAN,     .string = "cyan"},
{.color = LED_COLOR_CYAN,     .string = "CYN"},
{.color = LED_COLOR_MAGENTA,  .string = "magenta"},
{.color = LED_COLOR_MAGENTA,  .string = "MAG"},
{.color = LED_COLOR_WHITE,    .string = "white"},
{.color = LED_COLOR_WHITE,    .string = "WHT"},
};


static int _ReadLedArguments( tLedStateClass     state,
                                xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args);

void FreeLedDefaults(tLedDefaults * pDef)
{
  while(pDef != NULL)
  {
    tLedDefaults * pDel = pDef;
    pDef = pDef->pNext;
    free(pDel->ledName);
    free(pDel);
  }
}

void FreeLedNames(tLedNames * pNames)
{
  while(pNames != NULL)
  {
    tLedNames * pDel = pNames;
    pNames = pNames->pNext;
    free(pDel->ledName);
    free(pDel);
  }
}

void FreeDiagXml(tDiagXml * del)
{
  xmlFreeDoc(del->doc);
  free(del);
}

const char * GetStrOfLan(const char * lanPtr,tDiagXml * doc)
{
  char * ret = NULL;
  char * defLanPtr = NULL;

  while (doc->node != NULL)
  {
    if ((!xmlStrcmp(doc->node->name, (const xmlChar *)lanPtr)))
    {
      ret = (char *)xmlNodeListGetString(doc->doc, doc->node->xmlChildrenNode, 1);
      break;
    }
    if ((!xmlStrcmp(doc->node->name, (const xmlChar *)diagnostic_xml_default_lan)))
    {
      defLanPtr = (char *)xmlNodeListGetString(doc->doc, doc->node->xmlChildrenNode, 1);
    }
    doc->node = doc->node->next;
  }

  if(ret == NULL)
  {
    ret = defLanPtr;
  }

  return ret;
}

int GetStrings(tDiagXml * doc)
{
  int ret = -1;

  while (doc->node != NULL)
  {
    if ((!xmlStrcmp(doc->node->name, (const xmlChar *)"string")))
    {
      ret = 0;
      break;
    }
    doc->node = doc->node->next;
  }
  return ret;
}

int GetResetStrings(tDiagXml * doc)
{
  int ret = -1;

  while (doc->node != NULL)
  {
    if ((!xmlStrcmp(doc->node->name, (const xmlChar *)"rststr")))
    {
      ret = 0;
      break;
    }
    doc->node = doc->node->next;
  }
  return ret;
}

int  GetEvent(uint32_t id, tDiagXml * doc)
{
  int ret = -1;
  while (doc->node != NULL)
  {
    if ((!xmlStrcmp(doc->node->name, (const xmlChar *)"event")))
    {
      xmlChar *idStr = xmlGetProp(doc->node, (xmlChar*)"id");
      if(idStr != NULL)
      {
        uint32_t idIn = strtol((char*)idStr+1, NULL, 16);
        if(idIn == (id & 0x0000FFFF))
        {
          ret = 0;
          break;
        }
        free(idStr);
      }
    }
    doc->node = doc->node->next;
  }

  return ret;
}

int  GetClass(uint32_t id, tDiagXml * doc)
{
  int ret = -1;
  while (doc->node != NULL)
  {
    if ((!xmlStrcmp(doc->node->name, (const xmlChar *)"eventclass")))
    {
      xmlChar *rangeStr = xmlGetProp(doc->node, (xmlChar*)"class_range");
      if(rangeStr != NULL)
      {
        uint32_t range = strtol((char*)rangeStr+1, NULL, 16) << 16;
        if(range == (id & 0x3FFF0000))
        {
          ret = 0;
          break;
        }
        free(rangeStr);
      }
    }
    doc->node = doc->node->next;
  }

  return ret;
}

int GetRoot(tDiagXml * doc)
{

  if(   (doc->node == NULL)
     || (xmlStrcmp(doc->node->name, (const xmlChar *) "diagnostic")))
  {

    doc->node = xmlDocGetRootElement(doc->doc);

    if (doc->node == NULL)
    {
      fprintf(stderr,"empty document\n");
      return -1;
    }

    if (xmlStrcmp(doc->node->name, (const xmlChar *) "diagnostic"))
    {
      fprintf(stderr,"document of the wrong type, root node != diagnostic");
      return -1;
    }
  }
  return 0;
}

tDiagXml * ParseDoc(char *docname)
{

  tDiagXml * ret = NULL;

  if(access(docname, F_OK))
  {
    return NULL;
  }

  ret = malloc(sizeof(tDiagXml));
  if(ret == NULL)
  {
    return NULL;
  }

  ret->doc      = NULL;
  ret->node     = NULL;

  ret->doc = xmlParseFile(docname);

  if (ret->doc == NULL )
  {
    free(ret);
    return NULL;
  }



  return ret;
}

tLedStaticColor StrToColor(char * str)
{
  size_t i;
  for(i = 0;i < (sizeof(ledColors)/sizeof(tColorString)); i++)
  {
    if(!strcmp(ledColors[i].string,str))
    {
      break;
    }
  }

  return ledColors[i].color;
}

void CutcolorString(char ** ppLedState,char buffer[16])
{
  int i;
  char * pLedState = *ppLedState;
  for(i=0;*pLedState!=0;i++)
  {
    if(*pLedState=='_')
    {
      pLedState++;
      break;
    }
    buffer[i]=*(pLedState++);
  }
  buffer[i]=0;
  *ppLedState = pLedState;
}

void GetStaticColorFromString(char * pLedState,tLedInfo *ledInfo)
{
  char colorString[16];
  CutcolorString(&pLedState,colorString);
  ledInfo->vars.staticVar = StrToColor(colorString);
}

void GetSequenceStateFromString(char * pLedState,tLedInfo *ledInfo)
{
  char seqString[16];
  CutcolorString(&pLedState,seqString);
  ledInfo->state = LED_STATE_750_ERR;
  if(!strcmp(seqString,"RESET"))
  {
    ledInfo->vars.err750Var.errorArg=0xFFFF;
    ledInfo->vars.err750Var.errorCode=0xFFFF;
  }
  else
  {
    ledInfo->vars.err750Var.errorArg=0;
    ledInfo->vars.err750Var.errorCode=0;
  }
}

void GetBlinkingStateAndColorsFromString(char * pLedState,tLedInfo *ledInfo)
{
  char colorString[16];
  CutcolorString(&pLedState,colorString);
  if(!strcmp(colorString,"SEQ"))
  {
    GetSequenceStateFromString(pLedState,ledInfo);
  }
  else
  {
    ledInfo->state = LED_STATE_BLINK;
    ledInfo->vars.blinkVar.color1=StrToColor(colorString);
    if(*pLedState != 0)
    {
      ledInfo->vars.blinkVar.color2=StrToColor(pLedState);
    }
    else
    {
      ledInfo->vars.blinkVar.color2 = LED_COLOR_OFF;
    }
  }
  //ledInfo->vars.staticVar.color = StrToColor(colorString);
}

void GetFlashStateAndColorsFromString(char * pLedState,tLedInfo *ledInfo)
{
  char colorString[16];
  CutcolorString(&pLedState,colorString);
  ledInfo->state = LED_STATE_FLASH;
  ledInfo->vars.flashVar.flashColor=StrToColor(colorString);
  if(*pLedState != 0)
  {
    ledInfo->vars.flashVar.staticColor=StrToColor(pLedState);
  }
  else
  {
    ledInfo->vars.flashVar.staticColor = LED_COLOR_OFF;
  }
}

void GetLedStateAndColorFromString(const char * ledState,tLedInfo * ledInfo)
{
  char * pLedState = (char*)ledState;
  char stateString[16];
  CutcolorString(&pLedState,stateString);
  if(!strcmp(stateString,"STATIC"))
  {
    ledInfo->state = LED_STATE_STATIC;
    GetStaticColorFromString(pLedState,ledInfo);
  }
  else if(!strcmp(stateString,"BLINK"))
  {
    GetBlinkingStateAndColorsFromString(pLedState,ledInfo);
  }
  else if(!strcmp(stateString,"SPECIAL"))
  {
    ledInfo->state = LED_STATE_CAN;
    //GetSpecialStateAndColors(pLedState,ledInfo);
  }
  else if(!strcmp(stateString,"FLASH"))
  {
    ledInfo->state = LED_STATE_FLASH;
    GetFlashStateAndColorsFromString(pLedState,ledInfo);
  }
}

tLedAlias *  _AddAliasToList(tLedAlias * list, xmlChar * str)
{

  tLedAlias * add = malloc(sizeof(tLedAlias));

  add->alias = strdup((char*)str);
  add->pNext = NULL;

  if(list == NULL)
  {
    list = add;
  }
  else
  {
    tLedAlias * act = list;
    while(act->pNext != NULL)
    {
      act = act->pNext;
    }
    act->pNext = add;
  }

  return list;
}

tLedAlias * _GetLedAliases(xmlNodePtr ledNode)
{
  tLedAlias * list = NULL;

  ledNode = xmlFirstElementChild(ledNode);

  while(ledNode != NULL)
  {
    xmlChar * str = NULL;
    str = xmlNodeGetContent(ledNode);
    if(str != NULL)
    {
      if(!xmlStrcmp(ledNode->name, (const xmlChar *)"alias"))
      {
        list = _AddAliasToList(list, str);
      }
    }
    ledNode = xmlNextElementSibling(ledNode);
  }

  return list;
}

tLedAlias * _CopyAlias(tLedAlias * in)
{
  tLedAlias * ret = NULL;
  if(in != NULL)
  {
    tLedAlias * actIn = in;
    tLedAlias * actOut = NULL;
    ret = malloc(sizeof(tLedAlias));
    actOut=ret;
    actOut->alias = strdup(actIn->alias);
    actOut->pNext = NULL;
    actIn = actIn->pNext;
    while(actIn != NULL)
    {
      actOut->pNext = malloc(sizeof(tLedAlias));
      actOut = actOut->pNext;
      actOut->alias = strdup(actIn->alias);
      actOut->pNext = NULL;
      actIn = actIn->pNext;
    }
  }

  return ret;
}

void ParseLedDoc(char *docname, tLedBehavior * doc)
{
  xmlDocPtr  ledDoc;
  xmlNodePtr ledNode;

  if(doc == NULL)
  {
    fprintf(stderr,"Document must be a valid pointer \n");
    return;
  }

  ledDoc = xmlParseFile(docname);

  if (ledDoc == NULL )
  {
    fprintf(stderr,"Document not parsed successfully. \n");
    return;
  }
  ledNode = xmlDocGetRootElement(ledDoc);

  if (ledNode == NULL)
  {
    fprintf(stderr,"empty document\n");
    return;
  }
  if (xmlStrcmp(ledNode->name, (const xmlChar *) "ledconfig"))
  {
    fprintf(stderr,"document of the wrong type, root node != ledconfig");
    return;
  }

  ledNode = ledNode->xmlChildrenNode;
  while(ledNode != NULL)
  {
    if(!xmlStrcmp(ledNode->name, (const xmlChar *) "led"))
    {
      xmlChar * name     = xmlGetProp(ledNode, (xmlChar*)"name");
      xmlChar * nrStr    = xmlGetProp(ledNode, (xmlChar*)"number");
      xmlChar * stateStr = xmlGetProp(ledNode, (xmlChar*)"state");
      xmlChar * idStr    = xmlGetProp(ledNode, (xmlChar*)"id");
      tLedVariables trash;
      tLedNames     * newName = malloc(sizeof(tLedNames));
      tLedDefaults  * newDefault = malloc(sizeof(tLedDefaults));
      if(   (newName == NULL)
         || (newDefault == NULL))
      {
        xmlFreeDoc(ledDoc);
        fprintf(stderr,"memory error\n");
        return;
      }
      newName->ledNr = strtol((char *)nrStr, NULL, 10);
      newName->ledName = strdup((const char*)name);
      newName->alias = _GetLedAliases(ledNode);
      newName->pNext = doc->names;
      doc->names = newName;

      newDefault->ledNr = newName->ledNr;
      newDefault->ledName = strdup(newName->ledName);
      GetLedStateAndColorFromString((char *) stateStr,&(newDefault->info));
      newDefault->info.idInfo.id = strtol((char *)idStr, NULL, 16);
      printf("DEFAULT_ID: %X\n",newDefault->info.idInfo.id);
      newDefault->info.idInfo.info = NULL;

      _ReadLedArguments(newDefault->info.state,ledNode,
                                    &newDefault->info,&trash);


      newDefault->pNext = doc->defaults;
      doc->defaults = newDefault;
      if(nrStr != NULL)
      {
        free(nrStr);
      }
      if(name != NULL)
      {
        free(name);
      }
      if(stateStr != NULL)
      {
        free(stateStr);
      }
      if(idStr != NULL)
      {
        free(idStr);
      }
    }
    ledNode = ledNode->next;
  }
  xmlFreeDoc(ledDoc);
}

static int _GoToEvent(uint32_t id, tDiagXml * xmlDoc)
{
  int ok = 0;
  ok = GetRoot(xmlDoc);
  if(!ok)
  {
    xmlDoc->node = xmlDoc->node->xmlChildrenNode;
    ok = GetClass(id, xmlDoc);
  }
  if(!ok)
  {
    xmlDoc->node = xmlDoc->node->xmlChildrenNode;
    ok = GetEvent(id, xmlDoc);
  }

  return ok;
}

const char * GetStringOfId(uint32_t id, tDiagXml * xmlDoc, const char * lan)
{
  char *ret = NULL;
  int ok = 0;
  char * lanPtr = (char*)lan;

  if(xmlDoc==NULL)
  {
    return NULL;
  }
  ok = _GoToEvent(id, xmlDoc);
  if(!ok)
  {
    xmlDoc->node = xmlDoc->node->xmlChildrenNode;
    ok = GetStrings(xmlDoc);
  }
  if(!ok)
  {
    if(lanPtr == NULL)
    {
      lanPtr = (char*) diagnostic_xml_default_lan;
    }
    xmlDoc->node = xmlDoc->node->xmlChildrenNode;;
    ret = (char*) GetStrOfLan(lanPtr, xmlDoc);
  }



  return ret;
}

const char * GetResetStringOfId(uint32_t id, tDiagXml * xmlDoc, const char * lan)
{
  char *ret = NULL;
  int ok = 0;
  char * lanPtr = (char*)lan;

  ok = _GoToEvent(id, xmlDoc);

  if(!ok)
  {
    xmlDoc->node = xmlDoc->node->xmlChildrenNode;
    ok = GetResetStrings(xmlDoc);
  }
  if(!ok)
  {
    if(lanPtr == NULL)
    {
      lanPtr = (char*) diagnostic_xml_default_lan;
    }
    xmlDoc->node = xmlDoc->node->xmlChildrenNode;;
    ret = (char*) GetStrOfLan(lanPtr, xmlDoc);
  }

  return ret;
}

static int _ReadStaticArguments(xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args)
{
  //int v1;
  tLedStatic * ledVal = &(info->vars.staticVar);
  tLedStatic * ledArg = &(args->staticVar);
  node = xmlFirstElementChild(node);
  *ledArg = 0;
  while(node != NULL)
  {
    xmlChar * str = NULL;
    str = xmlNodeGetContent(node);
    if(str != NULL)
    {
      if(!xmlStrcmp(node->name, (const xmlChar *)"color"))
      {
        if(*str == '$')
        {
          *ledVal = LED_COLOR_OFF;
          *ledArg = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          *ledArg = 0;
          *ledVal = StrToColor((char*)str);
        }
      }
    }
    node = xmlNextElementSibling(node);
  }

  return 0;
}

static int _ReadBlinkArguments(xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args)
{
  int v1,v2;
  tLedBlink * ledVal = &(info->vars.blinkVar);
  tLedBlink * ledArg = &(args->blinkVar);
  node = xmlFirstElementChild(node);
  ledArg->color1=0;
  ledArg->color2=0;
  ledArg->time1=0;
  ledArg->time2=0;
  while(node != NULL)
  {
    xmlChar * str = NULL;
    str = xmlNodeGetContent(node);
    if(str != NULL)
    {
      if(!xmlStrcmp(node->name, (const xmlChar *)"on"))
      {
        if(*str == '$')
        {
          ledVal->time1 = LED_COLOR_OFF;
          ledArg->time1 = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->time1 = 0;
          ledVal->time1 = strtol((char*)str, NULL, 10);
        }
        v1 = 1;
      }
      if(!xmlStrcmp(node->name, (const xmlChar *)"off"))
      {
        if(*str == '$')
        {
          ledVal->time2 = LED_COLOR_OFF;
          ledArg->time2 = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->time2 = 0;
          ledVal->time2 = strtol((char*)str, NULL, 10);
        }
        v2 = 1;
      }
      if(!xmlStrcmp(node->name, (const xmlChar *)"color1"))
      {
        if(*str == '$')
        {
          ledVal->color1 = LED_COLOR_OFF;
          ledArg->color1 = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->color1 = 0;
          ledVal->color1 = StrToColor((char*)str);
        }
      }
      if(!xmlStrcmp(node->name, (const xmlChar *)"color2"))
      {
        if(*str == '$')
        {
          ledVal->color2 = LED_COLOR_OFF;
          ledArg->color2 = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->color2 = 0;
          ledVal->color2 = StrToColor((char*)str);
        }
      }
    }
    node = xmlNextElementSibling(node);
  }
  if(v1 && v2)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

static int _ReadFlashArguments(xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args)
{
  int v1;
  tLedFlash * ledVal = &(info->vars.flashVar);
  tLedFlash * ledArg = &(args->flashVar);
  node = xmlFirstElementChild(node);
  ledArg->flashColor=0;
  ledArg->flashTime=0;
  ledArg->staticColor=0;
  while(node != NULL)
  {
    xmlChar * str = NULL;
    str = xmlNodeGetContent(node);
    if(str != NULL)
    {
      if(!xmlStrcmp(node->name, (const xmlChar *)"on"))
      {
        if(*str == '$')
        {
          ledVal->flashTime = LED_COLOR_OFF;
          ledArg->flashTime = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->flashTime = 0;
          ledVal->flashTime = strtol((char*)str, NULL, 10);
        }
        v1 = 1;
      }
      if(!xmlStrcmp(node->name, (const xmlChar *)"flashcolor"))
      {
        if(*str == '$')
        {
          ledVal->flashColor = LED_COLOR_OFF;
          ledArg->flashColor = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->flashColor = 0;
          ledVal->flashColor = StrToColor((char*)str);
        }
      }
      if(!xmlStrcmp(node->name, (const xmlChar *)"staticcolor"))
      {
        if(*str == '$')
        {
          ledVal->staticColor = LED_COLOR_OFF;
          ledArg->staticColor = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->staticColor = 0;
          ledVal->staticColor = StrToColor((char*)str);
        }
      }
    }
    node = xmlNextElementSibling(node);
  }
  if(v1)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

static int _ReadCanArguments(xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args)
{
  int v1;
  tLedCan * ledVal = &(info->vars.canVar);
  tLedCan * ledArg = &(args->canVar);
  node = xmlFirstElementChild(node);
  ledArg->canCode=0;
  while(node != NULL)
  {
    xmlChar * str = NULL;
    str = xmlNodeGetContent(node);
    if(str != NULL)
    {
      if(!xmlStrcmp(node->name, (const xmlChar *)"blinkarg"))
      {
        if(*str == '$')
        {
          ledVal->canCode = 0;
          ledArg->canCode = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->canCode = 0;
          ledVal->canCode = strtol((char*)str, NULL, 10);
        }
        v1 = 1;
      }
    }
    node = xmlNextElementSibling(node);
  }
  if(v1)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

static int _Read750Arguments(xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args)
{
  int v1,v2;
  tLed750 * ledVal = &(info->vars.err750Var);
  tLed750 * ledArg = &(args->err750Var);
  node = xmlFirstElementChild(node);
  ledArg->errorArg=0;
  ledArg->errorCode=0;
  while(node != NULL)
  {
    xmlChar * str = NULL;
    str = xmlNodeGetContent(node);
    if(str != NULL)
    {
      if(!xmlStrcmp(node->name, (const xmlChar *)"blinkcode"))
      {
        if(*str == '$')
        {
          ledVal->errorCode = 0;
          ledArg->errorCode = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->errorCode = 0;
          ledVal->errorCode = strtol((char*)str, NULL, 10);
        }
        v1 = 1;
      }
    }
    if(str != NULL)
    {
      if(!xmlStrcmp(node->name, (const xmlChar *)"blinkarg"))
      {
        if(*str == '$')
        {
          ledVal->errorArg = 0;
          ledArg->errorArg = strtol((char*)str+1, NULL, 10);
        }
        else
        {
          ledArg->errorArg = 0;
          ledVal->errorArg = strtol((char*)str, NULL, 10);
        }
        v2 = 1;
      }
    }
    node = xmlNextElementSibling(node);
  }
  if(v1 && v2)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

static int _ReadLedArguments( tLedStateClass state,
                                xmlNodePtr  node,
                                tLedInfo * info,
                                tLedVariables  * args)

{
  int ret = 0;
  switch(state)
  {
    case LED_STATE_OFF:
        //NOT allowed
      break;
    case LED_STATE_STATIC:
      ret = _ReadStaticArguments(node,info,args);
      break;
    case LED_STATE_BLINK:
      ret = _ReadBlinkArguments(node,info,args);
      break;
    case LED_STATE_FLASH:
      ret = _ReadFlashArguments(node,info,args);
      break;
    case LED_STATE_CAN:
      ret = _ReadCanArguments(node,info,args);
      break;
    case LED_STATE_750_ERR:
      ret = _Read750Arguments(node,info,args);
      break;
    default:
      //empty
      break;
  }
  return ret;
}

static tLedEventList * _GetLedElement(tLedEventList ** pledList,uint32_t id)
{
  tLedEventList * pAct = *pledList;
  tLedEventList * res = NULL;

  while(pAct != NULL)
  {
    if(pAct->info.idInfo.id == id)
    {
      break;
    }
    pAct = pAct->pNext;
  }
  res = pAct;

  if(res == NULL)
  {
    res = malloc(sizeof(tLedEventList));
    if(res == NULL)
    {
      return NULL;
    }
    memset(res,0,sizeof(tLedEventList));
    res->pNext = *pledList;
    res->info.flags = LED_FLAG_EVENT_NEW;
    *pledList = res;
  }

  return res;
}

void InitLedInfo(tLedInfo * ledInfo)
{
  ledInfo->defaultIdInfo.id =0;
  ledInfo->defaultIdInfo.info = NULL;
  ledInfo->defaultIdInfo.refCount = 0;
  ledInfo->defaultState = LED_STATE_OFF;
  memset(&(ledInfo->defaultVars), 0 , sizeof(tLedVariables));
  ledInfo->flags = 0;
  ledInfo->idInfo.id =0;
  ledInfo->idInfo.info = NULL;
  ledInfo->idInfo.refCount = 0;
  ledInfo->pWarteschlange = NULL;
  ledInfo->setTime.tv_sec = 0;
  ledInfo->setTime.tv_usec =0;
  ledInfo->state = LED_STATE_OFF;
  memset(&(ledInfo->vars), 0 , sizeof(tLedVariables));

}

void GetSizeOfState(tLedStateClass state, size_t * szVar)
{
  switch(state)
  {
    case LED_STATE_STATIC:
      *szVar=sizeof(tLedStatic);
      break;
    case LED_STATE_BLINK:
      *szVar=sizeof(tLedBlink);
      break;
    case LED_STATE_FLASH:
      *szVar=sizeof(tLedFlash);
      break;
    case LED_STATE_750_ERR:
      *szVar=sizeof(tLed750);
      break;
    case LED_STATE_CAN:
      *szVar=sizeof(tLedCan);
      break;
    case LED_STATE_OFF:
      *szVar=0;
      break;
    default:
      break;
  }
}

void VarCopy(tLedVariables * dest,tLedVariables * src,tLedStateClass  state)
{
  size_t szVar;
  GetSizeOfState(state, &szVar);
  memcpy(dest,src,szVar);
}


static void _EventListReadLed(uint32_t id, tLedBehavior * ledBehavior, xmlNodePtr node, tLedEventList ** pledList)
{
  xmlChar * ledName    = xmlGetProp(node, (xmlChar*)"name");
  xmlChar * ledState   = xmlGetProp(node, (xmlChar*)"state");
  xmlChar * ledDefault = xmlGetProp(node, (xmlChar*)"default");
  int ledNr = -1;
  tLedInfo   info;
  tLedVariables args;
  uint8_t         flags = 0;
  tLedNames * ledEventName = NULL;

  InitLedInfo(&info);

  //testcode
  if(ledName != NULL && ledNr == -1)
  {
    tLedNames * ledNames = ledBehavior->names;
    while(ledNames != NULL)
    {
      if ((!xmlStrcmp(ledName, (const xmlChar *)ledNames->ledName)))
      {
        ledNr = ledNames->ledNr;
        ledEventName = ledNames;
        break;
      }
      else if(ledNames->alias != NULL)
      {
        tLedAlias * alias = ledNames->alias;
        do{
          if ((!xmlStrcmp(ledName, (const xmlChar *)alias->alias)))
          {
            ledNr = ledNames->ledNr;
            ledEventName = ledNames;
            break;
          }
          alias = alias->pNext;
        }while(alias != NULL);
      }
      ledNames = ledNames->pNext;
    }
    //free(ledName);
  }


  if(ledState != NULL)
  {
    GetLedStateAndColorFromString((char*)ledState,&info);
    free(ledState);
  }

  if(ledDefault != NULL)
  {
    flags |= LED_FLAG_SET_TO_DEFAULT;
    if ((!xmlStrcmp(ledDefault, (const xmlChar*) "only")))
    {
      flags |= LED_FLAG_DEFAULT_NOT_ADOPT;
    }
    free(ledDefault);
  }

  if((   (info.state == LED_STATE_STATIC)
      || (info.state == LED_STATE_BLINK)
      || (info.state == LED_STATE_FLASH)
      || (info.state == LED_STATE_750_ERR)
      || (info.state == LED_STATE_CAN))
      && (ledNr != -1)
     )
  {
    tLedEventList * newEvent = NULL;
    if(_ReadLedArguments(info.state, node, &info,&args))
    {
      return;
    }
    if(ledEventName == NULL)
    {
      return;
    }
    newEvent = _GetLedElement(pledList,id);
    if(newEvent == NULL)
    {
      return;
    }
    newEvent->info.idInfo.id    = id;
    newEvent->ledNr = ledNr;
    newEvent->ledName = strdup((char*)ledEventName->ledName);
    free(ledName);
    newEvent->alias = _CopyAlias(ledEventName->alias);
    newEvent->info.state = info.state;
    VarCopy(&(newEvent->info.vars),&(info.vars),info.state);
    VarCopy(&(newEvent->args),&(args),info.state);
    newEvent->info.flags &= LED_FLAG_EVENT_NEW;
    newEvent->info.flags |= flags | LED_FLAG_EVENT_MODIFIED;
    newEvent->file = pActualFile;

  }
}

static void _EventListReadEvent(uint32_t range, tLedBehavior * ledBehavior,xmlNodePtr node,tLedEventList ** pledList)
{
  xmlChar *idStr = xmlGetProp(node, (xmlChar*)"id");
  if(idStr != NULL)
  {
    uint32_t id = range | strtol((char*)idStr+1, NULL, 16);

    xmlChar *persistentStr = xmlGetProp(node, (xmlChar*)"persistent");
    if(persistentStr != NULL)
    {
      if ((!xmlStrcmp(persistentStr, (const xmlChar *)"yes")))
      {
        id |= DIAG_ID_SAVE_PERS;
      }
      else if ((!xmlStrcmp(persistentStr, (const xmlChar *)"none")))
      {
        id |= DIAG_ID_SAVE_NO;
      }
      free(persistentStr);
    }

    node = xmlFirstElementChild(node);
    while(node != NULL)
    {
      if ((!xmlStrcmp(node->name, (const xmlChar *)"led")))
      {
        _EventListReadLed(id, ledBehavior, node, pledList);
      }
      node = xmlNextElementSibling(node);
    }
    free(idStr);
  }
}


static void _EventListReadClass(tLedBehavior * ledBehavior,xmlNodePtr node, tLedEventList ** pledList)
{
  xmlChar *rangeStr = xmlGetProp(node, (xmlChar*)"class_range");
   if(rangeStr != NULL)
   {
     uint32_t range = strtol((char*)rangeStr+1, NULL, 16) << 16;
     node = xmlFirstElementChild(node);
     while(node != NULL)
     {
       if ((!xmlStrcmp(node->name, (const xmlChar *)"event")))
       {
         _EventListReadEvent(range , ledBehavior, node,pledList);
       }
       node = xmlNextElementSibling(node);
     }
     free(rangeStr);
   }
}

static void _ReadFileForList(tLedBehavior * ledBehavior,const char * filename, tLedEventList ** pledList)
{
  xmlTextReaderPtr reader;
  int (*pXmlReadFct)(xmlTextReaderPtr reader) = xmlTextReaderRead;

  (void)ledBehavior;
  (void)filename;
  (void)pledList;

  reader = xmlReaderForFile(filename, NULL, 0);
  if (reader != NULL)
  {
    int ok;
    while (1 == (ok = pXmlReadFct(reader)))
    {
      const xmlChar *name = xmlTextReaderConstName(reader);
      if (   (name != NULL)
          && ((!xmlStrcmp(name, (const xmlChar *)"eventclass"))))
      {
        if(pXmlReadFct != xmlTextReaderNext)
        {
          pXmlReadFct = xmlTextReaderNext;
        }
        xmlNodePtr node =  xmlTextReaderExpand (reader);
        _EventListReadClass(ledBehavior, node, pledList );
      }
    }
    xmlFreeTextReader(reader);
    if (ok != 0)
    {
        fprintf(stderr, "%s : failed to parse\n", filename);
    }
  }
  else
  {
    fprintf(stderr, "Unable to open %s\n", filename);
  }
}

int CreateLedEventList(tLedBehavior * ledBehavior, tLedFiles * ledFiles, tLedEventList ** ledEvents)
{
  assert(ledBehavior);
  assert(ledFiles);
  assert(ledEvents);
  tLedFiles * pAct = ledFiles;

  while(pAct)
  {
    if(!access(pAct->path,R_OK))
    {
      struct stat buf;
      //easy but we must take care of the use of this construct now!
      pActualFile = pAct;
      _ReadFileForList(ledBehavior, pAct->path, ledEvents);
      if(!stat(pAct->path, &buf))
      {
        pAct->mtim.tv_sec = buf.st_mtim.tv_sec;
        pAct->mtim.tv_nsec = buf.st_mtim.tv_nsec;
      }
    }
    pAct = pAct->pNext;
  }


  return 1;
}


int UpdateLedEventList(tLedBehavior * ledBehavior, tLedFiles * ledFiles, tLedEventList ** ledEvents)
{
  assert(ledBehavior);
  assert(ledFiles);
  assert(ledEvents);
  tLedFiles * pAct = ledFiles;
  int ret = 0;

  while(pAct)
  {
    struct stat buf;
    pAct->flags &= ~LED_FILE_FLAG_REREAD;
    if(   (!stat(pAct->path, &buf))
       && (   (pAct->mtim.tv_sec != buf.st_mtim.tv_sec)))
    {
      puts("upgrade led def");
      ret = 1;
      pAct->flags |= LED_FILE_FLAG_REREAD;
      //easy but we must take care of the use of this construct now!
      pActualFile = pAct;
      _ReadFileForList(ledBehavior, pAct->path, ledEvents);
      if(!stat(pAct->path, &buf))
      {
        pAct->mtim.tv_sec = buf.st_mtim.tv_sec;
        pAct->mtim.tv_nsec = buf.st_mtim.tv_nsec;
      }
    }
    pAct = pAct->pNext;
  }


  return ret;
}
