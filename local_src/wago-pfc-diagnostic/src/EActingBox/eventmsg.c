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
///  \file     eventmsg.c
///
///  \version  $Revision: 1 $
///
///  \brief    Dieses Programm erzeugt eine Eventnachricht, die im
///            led-Error-server einen Protokolleintrag erzeugt.
///
///  \author   Hans Florian Scholz : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "eactingbox.h"



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
// Local variables
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
  //int i;

  printf("\n* Send Event-Message as ID *\n\n");
  printf("Usage: eventmsg [option] <ID>\n\n");

  printf("option:\n");
  printf(" -n <NAME>\tUse this <NAME> for the message sending program-name\n");
  printf(" -r\t\tMark message as reseted\n");
  printf(" -h\t\tPrint this help\n");
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
int eventmsg_main(int argc, char **argv)
{
  log_tEventId id;
  int c;
  char        defname[] = "eventmsg";
  char        *name = defname;
  bool set = true;
  bool usage = false;

#if 0
  if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
  {
    ShowHelp();
    return -1;
  }

  if(argc != 3)
  {
    printf("WRONG NUMBER OF ELEMENTS\n\n");
    ShowHelp();
    return -1;
  }
#endif

  while ((c = getopt(argc, argv, "n:rh")) != -1)
    {
      switch(c)
      {
        case 'n':
            name = optarg;
            break;
        case 'r':
            set = false;
            break;
        case 'h':
            usage = true;
            break;
        case '?':
        default:
            fprintf(stderr,"unknown arg %c\n", optopt);
            usage = true;
            break;
      }
    }

  if(argc <= (optind))
  {
    printf("WRONG NUMBER OF ELEMENTS: %d - %d\n",argc, optind);
    usage = true;
  }
  if((usage))
  {
    ShowHelp();
    return -1;
  }


  id = strtoll(argv[optind], NULL, 16);

  log_EVENT_Init(name);

  log_EVENT_LogId(id, set);

  return 0;
}
//---- End of source file ------------------------------------------------------
