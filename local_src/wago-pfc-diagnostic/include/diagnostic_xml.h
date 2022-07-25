//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
/*
 * diagnostig_xml.h
 *
 * TODO: make a real Library including code Style Rules!!!
 *  Created on: 28.05.2012
 *      Author: flormel
 */

#ifndef DIAGNOSTIG_XML_H_
#define DIAGNOSTIG_XML_H_

#include <stdint.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <LedCtl_API.h>
#include <ledserver_API.h>
//#include "sequence_flash.h"
//#include "sequence_can.h"
//#include "sequence_750.h"
#include <glib.h>




#define LED_FLAG_SET_TO_DEFAULT       0x01
#define LED_FLAG_DEFAULT_NOT_ADOPT    0x02
#define LED_FLAG_EVENT_NEW            0x04
#define LED_FLAG_EVENT_MODIFIED       0x08

#define LED_FLAG_DEFAULT_VAL 0x01

#define LED_FILE_FLAG_REREAD          0x01

#define DIAGNOSTIC_XML_DOCUMENT         "/etc/specific/diagnostic.xml"
#define DIAGNOSTIC_XML_DOCUMENT_CUSTOM  "/tmp/cstdiagnostic.xml"


extern const char diagnostic_xml_default_lan[];

typedef struct {
    uint32_t   id;
    GList    * info;
    uint32_t   refCount;
}tIdInfo;


typedef struct stWarteschlange{
    tIdInfo         idInfo;
    uint16_t        var1;              /* first Blinkkargument Errorcode */
    uint16_t        var2;              /* second Blinkargiment Errorarg */
    struct timeval  setTime;
    struct stWarteschlange *pNext;
}tWarteschlange;

typedef union
{
    tLedStatic staticVar;
    tLedBlink  blinkVar;
    tLedFlash  flashVar;
    tLed750    err750Var;
    tLedCan    canVar;
}tLedVariables;

typedef struct {
   tLedStateClass  defaultState;
   tLedVariables   defaultVars;
   tIdInfo         defaultIdInfo;
   tIdInfo         idInfo;
   uint8_t         flags;
   struct timeval  setTime;
   tWarteschlange *pWarteschlange;     /* waiting-queue pointer */
}tLedData;


typedef struct {
   tLedStateClass  state;
   tLedVariables   vars;
   tLedStateClass  defaultState;
   tLedVariables   defaultVars;
   tIdInfo         defaultIdInfo;
   tIdInfo         idInfo;
   uint8_t         flags;
   struct timeval  setTime;
   tWarteschlange *pWarteschlange;     /* waiting-queue pointer */
}tLedInfo;

typedef struct stLedAlias
{
    char * alias;
    struct stLedAlias * pNext;
}tLedAlias;

typedef struct stLedNames
{
    int    ledNr;
    char * ledName;
    tLedAlias     * alias;
    struct stLedNames * pNext;
}tLedNames;


typedef struct stLedDefaults
{
    int             ledNr;
    char          * ledName;
//    tLedStateClass  state;
//    uint16_t        var1;
//    uint16_t        var2;
//    tIdCtrlInfo     idInfo;
    tLedInfo          info;
    struct stLedDefaults * pNext;
}tLedDefaults;

typedef struct {
  xmlDocPtr      doc;
  xmlNodePtr     node;
  //tLedNames    * names;
  //tLedDefaults * defaults;
}tDiagXml;

typedef struct {
  tLedNames    * names;
  tLedDefaults * defaults;
}tLedBehavior;

typedef struct stLedFiles tLedFiles;
struct stLedFiles {
    char * path;
    struct timespec mtim;
    uint8_t flags;
    tLedFiles * pNext;
};


typedef struct stLedEventList
{
   int               ledNr;
   char            * ledName;
   tLedAlias       * alias;
//   uint32_t          id;
//   uint8_t           state;
//   uint16_t          var1;               /* First Blinkargument */
//   uint16_t          var2;
//   uint32_t          arg1;
//   uint32_t          arg2;
//   uint8_t           flags;
   tLedInfo          info;
   tLedVariables     args;
   int               handle;
   tLedFiles       * file;
   struct stLedEventList * pNext;
}tLedEventList;

typedef struct stLedStates {
   uint8_t         state;
   char            name[64];
}tLedStates;

extern tLedStates ledStates[];

tDiagXml * ParseDoc(char *docname);
void ParseLedDoc(char *docname, tLedBehavior * doc);
void FreeDiagXml(tDiagXml * del);
const char * GetStringOfId(uint32_t id, tDiagXml * xmlDoc, const char * lan);
const char * GetResetStringOfId(uint32_t id, tDiagXml * xmlDoc, const char * lan);
//tLedEventList * CreateLedEventList(tDiagXml *);
int CreateLedEventList(tLedBehavior * ledBehavior, tLedFiles * ledFiles, tLedEventList ** ledEvents);
void FreeLedDefaults(tLedDefaults * pDef);
void FreeLedNames(   tLedNames * pNames);
int UpdateLedEventList(tLedBehavior * ledBehavior, tLedFiles * ledFiles, tLedEventList ** ledEvents);

//dirty
void InitLedInfo(tLedInfo * ledInfo);
void VarCopy(tLedVariables * dest,tLedVariables * src,tLedStateClass  state);
void GetSizeOfState(tLedStateClass state, size_t * szVar);

#endif /* DIAGNOSTIG_XML_H_ */
