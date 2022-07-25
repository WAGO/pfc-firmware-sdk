//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
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
#include <diagnostic/diagnostic_xml_API.h>
#include <LedCtl_API.h>

int DIAGXML_GetCstDiagDoc(xmlDocPtr *doc)
{

  if(!access(DIAGNOSTIC_XML_DOCUMENT_CUSTOM,W_OK|R_OK))
  {
    *doc = xmlParseFile(DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
    if (*doc == NULL)
    {
      fprintf(stderr, "Error: unable to parse file \"%s\"\n", DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
      return -1;
    }
  }
  else
  {
    xmlNodePtr root_node = NULL;
    *doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "diagnostic");
    //xmlAddChild(root_node, xmlNewText(BAD_CAST "\n"));
    xmlDocSetRootElement(*doc, root_node);
  }

   return 0;

}

static void _UpdateExisingIdString(xmlNodeSetPtr nodes, const xmlChar * string)
{
  int size;
  int i;

  assert(string);
  size = (nodes) ? nodes->nodeNr : 0;

  for(i = size - 1; i >= 0; i--)
  {
    assert(nodes->nodeTab[i]);

    xmlNodeSetContent(nodes->nodeTab[i], string);

    if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
        nodes->nodeTab[i] = NULL;
  }
}

static xmlNodePtr _GetNodePtr(xmlNodeSetPtr nodes)
{
  xmlNodePtr node = NULL;
  int size;
  int i;
  size = (nodes) ? nodes->nodeNr : 0;

  for(i = size - 1; i >= 0; i--)
  {
    assert(nodes->nodeTab[i]);

    //xmlNodeSetContent(nodes->nodeTab[i], string);

    if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
    {
      node = nodes->nodeTab[i];
      nodes->nodeTab[i] = NULL;
    }
  }
  xmlAddChild(node, xmlNewText(BAD_CAST "  "));
  return node;
}

static xmlNodePtr _NewClassNode(xmlDocPtr doc, uint16_t classId, const xmlChar * className)
{
  xmlNodePtr node = NULL;
  xmlNodePtr root_node = NULL;
  xmlChar strClassId[8];

  root_node = xmlDocGetRootElement(doc);
  xmlAddChild(root_node, xmlNewText(BAD_CAST "\n  "));
  node      = xmlNewChild(root_node, NULL, BAD_CAST "eventclass",NULL);
  xmlStrPrintf(strClassId, 8,(const char *) "x%.4X",classId);
  xmlNewProp(node, BAD_CAST "class_range", strClassId);
  xmlNewProp(node, BAD_CAST "name", className);
  xmlAddChild(node, xmlNewText(BAD_CAST "\n    "));
  return node;
}

static xmlNodePtr _NewEventNode(uint16_t eventId,const xmlChar * name, const xmlChar * string,const xmlChar * lan)
{
  xmlNodePtr event_node;
  xmlChar strEventId[8];
  xmlNodePtr string_node = NULL;
//  xmlNodePtr en_node = NULL;

  event_node = xmlNewNode(NULL, BAD_CAST "event");
  xmlStrPrintf(strEventId, 8, (const char *) "x%.4X",eventId);
  xmlNewProp(event_node, BAD_CAST "id", strEventId);
  xmlNewProp(event_node, BAD_CAST "name", name);

  xmlAddChild(event_node, xmlNewText(BAD_CAST "\n      "));

  string_node = xmlNewChild(event_node, NULL, BAD_CAST "string",NULL);
  xmlAddChild(string_node, xmlNewText(BAD_CAST "\n        "));
  (void) xmlNewTextChild(string_node, NULL, lan, string);

  xmlAddChild(string_node, xmlNewText(BAD_CAST "\n      "));
  xmlAddChild(event_node, xmlNewText(BAD_CAST "\n    "));

  return event_node;
}

static void _FormatRootElement(xmlDocPtr doc)
{
  xmlNodePtr root = xmlDocGetRootElement(doc);
  xmlNodePtr lastChild = xmlGetLastChild(root);

  if(lastChild->type != XML_TEXT_NODE)
  {
    xmlAddChild(root, xmlNewText(BAD_CAST "\n"));
  }
}


int DIAGXML_UpdateIdEntryString(xmlDocPtr doc,
                                 uint32_t id,
                                 const xmlChar *lan,
                                 const xmlChar * string)
{
  uint16_t classId = id >> 16;
  uint16_t eventId = id & 0xFFFF;
  xmlChar  xpathExpr[128];
  xmlXPathContextPtr xpathCtx;
  xmlXPathObjectPtr xpathObj;

  //first search for the full expression;
  xmlStrPrintf(xpathExpr, 128, (const char *) "//eventclass[@class_range = \"x%.4X\"]/event[@id = \"x%.4X\"]/string/%s",
               classId, eventId, lan);

  xpathCtx = xmlXPathNewContext(doc);
  if(xpathCtx == NULL)
  {
    fprintf(stderr,"Error: unable to create new XPath context\n");
    return -1;
  }
  /* Evaluate xpath expression */
  xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
  if(xpathObj == NULL)
  {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
    return -1;
  }

  if(xpathObj->nodesetval != NULL)
  {
    _UpdateExisingIdString(xpathObj->nodesetval,string);
  }
  else
  {
    xmlNodePtr class_node = NULL;
    xmlNodePtr event_node = NULL;
    xmlChar strBuffer[64];
    xmlXPathFreeObject(xpathObj);
    //find eventclass
    xmlStrPrintf(xpathExpr, 128,(const char *) "//eventclass[@class_range = \"x%.4X\"]",
                 classId);

    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL)
    {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
      return -1;
    }
    if(    (xpathObj->nodesetval != NULL)
       &&  (xpathObj->nodesetval->nodeNr > 0 ))
    {
      class_node = _GetNodePtr(xpathObj->nodesetval);
    }
    else
    {
      class_node = _NewClassNode(doc,classId, BAD_CAST "IEC-Custom");
    }
    xmlStrPrintf(strBuffer, 64, (const char *) "cst_event_x%.4X",eventId);
    event_node = _NewEventNode(eventId, strBuffer, string, lan);
    xmlAddChild(class_node, event_node);
    xmlAddChild(class_node, xmlNewText(BAD_CAST "\n  "));
    _FormatRootElement(doc);
  }
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  return 0;
}

void DIAGXML_SaveCstDiagDoc(xmlDocPtr doc)
{
  xmlSaveFormatFileEnc(DIAGNOSTIC_XML_DOCUMENT_CUSTOM_TMP, doc, "UTF-8", 1);

  if(rename (DIAGNOSTIC_XML_DOCUMENT_CUSTOM_TMP, DIAGNOSTIC_XML_DOCUMENT_CUSTOM))
  {
      fprintf(stderr, "Can't rename file %s.\n", DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
  }
  
}

int DIAGXML_RemoveCstDiagDoc(void)
{
  return unlink(DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
}
