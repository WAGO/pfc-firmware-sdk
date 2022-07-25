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
///  \file     test_main.c
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   Hans Florian Scholz : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "eactingbox.h"

static int ShowHelp(void);

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef int(*tSubFunction)(int argc, char **argv);

typedef struct stPrograms{
    char    name[32];
    //int     (*function)(int argc, char **argv);
    tSubFunction function;
}tProgramms;
//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
const tLEDstate states[] = {{LEDCTL_S_STATIC_OFF,    "OFF"},
                            {LEDCTL_S_STATIC_GRN,    "GRN"},
                            {LEDCTL_S_STATIC_RED,    "RED"},
                            {LEDCTL_S_STATIC_YLW,    "YLW"},
                            {LEDCTL_S_BLINK_GRN,     "BLINK_GRN"},
                            {LEDCTL_S_BLINK_RED,     "BLINK_RED"},
                            {LEDCTL_S_BLINK_YLW,     "BLINK_YLW"},
                            {LEDCTL_S_BLINK_GRN_RED, "BLINK_GRN_RED"},
                            {LEDCTL_S_BLINK_GRN_YLW, "BLINK_GRN_YLW"},
                            {LEDCTL_S_BLINK_RED_YLW, "BLINK_RED_YLW"},
                            {LEDCTL_S_BLINK_OFF_GRN, "BLINK_OFF_GRN"},
                            {LEDCTL_S_BLINK_OFF_RED, "BLINK_OFF_RED"},
                            {LEDCTL_S_BLINK_OFF_YLW, "BLINK_OFF_YLW"},
                            {LEDCTL_S_BLINK_RED_GRN, "BLINK_RED_GRN"},
                            {LEDCTL_S_BLINK_YLW_GRN, "BLINK_YLW_GRN"},
                            {LEDCTL_S_BLINK_YLW_RED, "BLINK_YLW_RED"},
                            {LEDCTL_S_BLINK_SEQ_750, "ERR_SEQ_750"},
                            {LEDCTL_S_FLASH_GRN_OFF, "FLASH_GRN_OFF"},
                            {LEDCTL_S_FLASH_RED_OFF, "FLASH_RED_OFF"},
                            {LEDCTL_S_FLASH_YLW_OFF, "FLASH_YLW_OFF"},
                            {LEDCTL_S_FLASH_GRN_RED, "FLASH_GRN_RED"},
                            {LEDCTL_S_FLASH_GRN_YLW, "FLASH_GRN_YLW"},
                            {LEDCTL_S_FLASH_RED_GRN, "FLASH_RED_GRN"},
                            {LEDCTL_S_FLASH_RED_YLW, "FLASH_RED_YLW"},
                            {LEDCTL_S_FLASH_YLW_GRN, "FLASH_YLW_GRN"},
                            {LEDCTL_S_FLASH_YLW_RED, "FLASH_YLW_RED"},
                            {LEDCTL_S_FLASH_OFF_GRN, "FLASH_OFF_GRN"},
                            {LEDCTL_S_FLASH_OFF_RED, "FLASH_OFF_RED"},
                            {LEDCTL_S_FLASH_OFF_YLW, "FLASH_OFF_YLW"},
                            {LEDCTL_S_BLINK_SPECIAL, "SPECIAL_BLINK"},
                            {LEDCTL_S_DO_NOT_SET,    "NOT_SET_LED"},
                            {LEDCTL_S_ERROR,         "LED_ERROR"}};


tProgramms programs[] = {

                         {"eventmsg",     eventmsg_main},
                         {"getidstate",   getidstate_main},
                         {"getledstate",  getledstate_main},
                         {"decodeid",     decodeid_main},
                         {"logforward",   logforward_main},
                         {"addcstdiag",   addcstdiag_main},
                         {"eactingbox",   (tSubFunction)ShowHelp}};


static int ShowHelp(void)
{
  int i;
  printf("\n* WAGO Diagnostic and LED Control Binary *\n\n");
  printf("Usage: eactingbox <programname> [<arguments>]\n\n");
  printf("Available programs: %s", programs[0].name);
  for(i = 1; programs[i].function != (tSubFunction)ShowHelp; i++ )
  {
    printf("; %s", programs[i].name);
  }
  printf("\n");

  return 0;
}


/*
 *-- Function: main ----------------------------------------------
 * 
 *  Diese Funktion wertet den Ersten übergebenen Parameter aus und gibt die
 *  übrigen an das Entsprechende Programm weiter
 * 
 * 
 *  \return <description of the return values of the function>
 *------------------------------------------------------------------------------
 */
int main(int argc, char **argv)
{
  char *pname;
  int i,j;

  pname = strrchr(argv[0],'/');
  if(pname == NULL)
  {
    pname=argv[0];
  }
  else
  {
    pname++;
  }
  i=0;
  while(strcmp(programs[i].name,pname) != 0)
  {
    if(programs[i].function == (tSubFunction)ShowHelp)
      break;
    i++;
  }
  if(programs[i].function == (tSubFunction)ShowHelp)
  {
    if(argc >= 2)
    {
      //strcpy(name,argv[1]);
      pname=argv[1];
      j=0;
      while(strcmp(programs[j].name,pname) != 0 && j<i)
      {
        j++;
      }
      i=j;
      argv = &argv[1];
      argc--;
    }
  }
  return (programs[i].function)(argc,argv);


}
//---- End of source file ------------------------------------------------------
