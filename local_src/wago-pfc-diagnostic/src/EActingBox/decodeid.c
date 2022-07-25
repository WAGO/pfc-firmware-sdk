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
///  \file     clearLog.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "eactingbox.h"
#include "parse_log.h"
#include "diagnostic_xml.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

/*
 *-- Function: ShowHelp ---------------------------------------------------
 *
 *  Zeigt den Hilfe-Text für das Programm an
 *
 *------------------------------------------------------------------------------
 */
static void ShowHelp(void)
{
  printf("\n* Decode ID-Logfile *\n\n");
  printf("Usage: decodeid [lan]\n\n");

  printf("Uses input over Stdin.\n");

  printf("Call it in conjunction with cat or tail etc.\n");

  printf("e.g.: # cat /varlog/wago/wagolog.log | decodeid /etc/specific/diagnostic.xml\n");


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
int decodeid_main(int argc, char **argv)
{
    char buffer[4096];
    tDiagXml * docs[2];
    char * lan = NULL;
    int exitval = -1;

    if(argc >= 2)
    {
      if((!strcmp(argv[1], "-h"))
         ||(!strcmp(argv[1], "--help")))
      {
        ShowHelp();
        return 1;
      }
      lan = argv[1];
    }

    docs[0] = ParseDoc(DIAGNOSTIC_XML_DOCUMENT_CUSTOM);
    docs[1] = ParseDoc(DIAGNOSTIC_XML_DOCUMENT);



    exitval = 0;
    while(NULL != fgets(buffer, 4096, stdin))
    {
      tLogData * logData = ParseLogLine(buffer);
      const char * string = NULL;
      bool resetStr = false;

      if(logData != NULL)
      {
        int i;
        for(i = 0; i < 2; i++)
        {
          if(docs[i] != NULL)
          {
            if(logData->set == true)
            {
              //puts("setted");
              string = GetStringOfId(logData->id, docs[i], lan);
              if(string != NULL)
              {
                break;
              }
            }
            else
            {
              //puts("Resetted");
              string = GetResetStringOfId(logData->id, docs[i], lan);
              if(string != NULL)
              {
                resetStr = true;
                break;
              }
              string = GetStringOfId(logData->id, docs[i], lan);
              if(string != NULL)
              {
                break;
              }

            }
          }
        }

        if(string != NULL)
        {
          puts(MergeLog(string, logData, resetStr));
        }
        else
        {
          printf("%s",buffer);
        }
      }
      else
      {
        printf("%s",buffer);
      }
    }
    return exitval;
}
//---- End of source file ------------------------------------------------------
