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
///  \file     diagnostic_xml_API.h
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef DIAGNOSTIC_XML_API_H_
#define DIAGNOSTIC_XML_API_H_

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <stdint.h>


#define DIAGNOSTIC_XML_DOCUMENT         "/etc/specific/diagnostic.xml"
#define DIAGNOSTIC_XML_DOCUMENT_CUSTOM  "/tmp/cstdiagnostic.xml"
#define DIAGNOSTIC_XML_DOCUMENT_CUSTOM_TMP  "/tmp/cstdiagnostic.xml.tmp"

int  DIAGXML_GetCstDiagDoc(      xmlDocPtr *);
int  DIAGXML_UpdateIdEntryString(xmlDocPtr,
                                 uint32_t,
                                 const xmlChar *,
                                 const xmlChar *);
void DIAGXML_SaveCstDiagDoc(     xmlDocPtr);
int  DIAGXML_RemoveCstDiagDoc(   void);

#endif /* DIAGNOSTIC_XML_API_H_ */
//---- End of source file ------------------------------------------------------
