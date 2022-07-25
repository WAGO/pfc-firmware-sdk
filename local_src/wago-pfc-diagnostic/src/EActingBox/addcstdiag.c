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
///  \file     addcstdiag.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <diagnostic/diagnostic_xml_API.h>


/*
 *-- Function: ShowHelp ---------------------------------------------------
 *
 *  Zeigt den Hilfe-Text für das Programm an
 *
 *------------------------------------------------------------------------------
 */
static void ShowHelp(void)
{
  printf("\n* Add ID to Custom diagnostigID-File *\n\n");
  printf("Usage: addcstdiag.c <ID> \"<String>\"\n\n");

  printf("ID must be an ID value in HEX or as Integer\n");

  printf("TAKE CARE: /tmp/cstdiagostic.xml will be deleted at next reboot\n");

  printf("e.g.: 0x0815 or 2069\n");
}

/*
 *-- Function: eventmsg_main ---------------------------------------------------
 *
 *  Main-Funktion für eventmsg
 *
 *  \param argc
 *  \param argv
 *
 *  \return <description of the return values of the function>
 *------------------------------------------------------------------------------
 */
int addcstdiag_main(int argc, char **argv)
{
    uint32_t id;
    xmlDocPtr doc;
   // char * lan = NULL;
    int exitval = -1;

    if(  (  (argc < 2)
          &&(  (!strcmp(argv[1], "-h"))
             ||(!strcmp(argv[1], "--help"))))
       ||(argc != 3))
    {
      ShowHelp();
      return 1;
    }

    id = strtoll(argv[1], NULL, 0);
    id &= 0x0000ffff;
    id |= 0x00050000;
    /* Init libxml */
    xmlInitParser();

    DIAGXML_GetCstDiagDoc(&doc);
    DIAGXML_UpdateIdEntryString(doc, id, BAD_CAST "en", BAD_CAST argv[2]);
    DIAGXML_SaveCstDiagDoc(doc);


    xmlFreeDoc(doc);

    /* Shutdown libxml */
    xmlCleanupParser();

    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();

    return exitval;
}
//---- End of source file ------------------------------------------------------
