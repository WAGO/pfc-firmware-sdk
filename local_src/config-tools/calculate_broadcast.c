//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2014 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     calculate_broadcast.c
///
///  \version  $Revision: 59621 $1
///
///  \brief    
///
///  \author   Stefanie Meihöfe, AGa : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_tool_lib.h"
#include "libnet/ct_libnet.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

// show errors on screen? Normally not, because they would be also displayed e.g. on WBM
#define SHOW_ERRORS                     0

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


void ShowHelpText(void)
    //
    // Show describtion and usage of program on stdout
    //
{
    printf("\n* Calculate broadcast from IP-address and subnet-mask *\n\n");
    printf("Usage: calculate_broadcast <IP-address> <subnet-mask>\n\n");
    printf("\n");
}


int main(int    argc, 
        char** argv)
{
    int   status            = SUCCESS;

    // help-text requested?
    if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
    {
        ShowHelpText();
    }
    else if(argc == 3)
    {
        char broadcast[strlen("255.255.255.255") + 1];

        status = ct_libnet_calculate_broadcast(argv[1], argv[2], broadcast, sizeof(broadcast));

        if(SUCCESS == status)
        {
            printf("%s", broadcast);
        }
    }
    else
    {
        status = INVALID_PARAMETER;
    }

#if SHOW_ERRORS
    if(status != SUCCESS)
    {
        ShowErrorText(status, "");
        ShowHelpText();
    }
#endif

    return(status);
}
