//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     print_program_output.c
///
///  \version  $Revision: 65689 $1
///
///  \brief    Get the output-string of another program as input-parameter and
///            write it to stdout.
///            This program is useful if the outputs of another programm where
///            written into a variable by bash, and the output includes several
///            lines. If this variable is written to stdout (or another file) 
///            by bash, the line-feeds disapear. This program shows the 
///            linefeeds correctly.
///
///  \author   Stefanie Meihöfer : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

// boolean
#define TRUE                             1
#define FALSE                            0


// error-numbers
#define SUCCESS                    0
#define MISSING_PARAMETER         -1
#define INVALID_PARAMETER         -2
#define FILE_OPEN_ERROR           -3
#define FILE_CLOSE_ERROR          -4
#define NOT_FOUND                 -5 


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


int main(int    argc, 
         char** argv)
{
  if(argc > 1) printf("%s\n", argv[1]);

  return(SUCCESS);
}
