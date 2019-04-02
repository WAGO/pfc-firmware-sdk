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
///  \file     dp_master.c
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   Rolf Schulenburg: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <menu.h>
#include <sdi.h>

#include "dpm_demo.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// Typedefs
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------------------------------------------------

//-- Function: safe_malloc -----------------------------------------------------------------
///
///  malloc wrapper that aborts the execution on error.
///
/// \param size Number of bytes to allocate.
///
/// \return Pointer to allocated memory.
//------------------------------------------------------------------------------------------
void *safe_malloc(size_t size)
{
   void * const p = malloc(size);

   if(NULL == p)
   {
      exit(EXIT_FAILURE);
   }

   return p;
}

//-- Function: sig_handler -----------------------------------------------------------------
///
///  malloc wrapper that aborts the execution on error.
///
/// \param size Number of bytes to allocate.
///
///  \return  none
//------------------------------------------------------------------------------------------
void sig_handler(int signo)
{

  switch(signo)
  {
    case SIGKILL  :
    case SIGHUP   :
    case SIGTERM  :
                      exit(0);
      break;
    default:
      break;
  }
  return;
}



//-- Function: main -------------------------------------------------------------------
///
///  This function prints out a collection of new menu fracments
///
///  \param       argc          NOT USED
///  \param       argv[0]       NOT USED
///  \param       argv[1]       --testmode_start: direct start of batch processing
///                             used for factory test
///  \return  error value
///
//------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{


  // catch signals
  signal(SIGKILL, sig_handler);
  signal(SIGHUP,  sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);

  menu_Print();

  if(argc != 1)
  {
    if(strcmp("-manual",argv[1] ) == ZERO)
    {
      menu_Manual();
    }
    else
    {
      printf("Error: Only <-manual> argument allowed.\n");
    }
  }
  sdi_Automatic();
  menu_Manual();




  return 0;
}





















//---- End of source file ------------------------------------------------------
