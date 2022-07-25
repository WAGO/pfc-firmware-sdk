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
///  \file     get_typelabel_value.c
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <typelabel_API.h>


//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------
typedef struct {
  uint8_t printNames:1;
  uint8_t verbose:1;
  uint8_t all:1;
  uint8_t usage:1;
  uint8_t unused:4;
}tOptions;

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
static const char optlist[] = "nvah";

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


//-- Function: _ParseOptions ---------------------------------------------------
///
/// Parse Command-Line Options to tOptions Instancy
///
/// \param argc: number of cmd-line parameter
/// \param argv: cmd-line parameter-list
///
/// \return filled instancy of tOptions
//------------------------------------------------------------------------------
static tOptions _ParseOptions(int argc, char ** argv)
{
  tOptions ret;
  int c;
  memset(&ret,0,sizeof(ret));

  while ((c = getopt(argc, argv, optlist)) != -1)
  {
    switch(c)
    {
      case 'n':
          ret.printNames = 1;
          break;
      case 'v':
          ret.verbose = 1;
          break;
      case 'h':
          ret.usage = 1;
          break;
      case 'a':
          ret.all = 1;
          break;
          break;
      case '?':
      default:
          fprintf(stderr,"unknown arg %c\n", optopt);
          break;
    }
  }

  return ret;
}

//-- Function: _Usage ---------------------------------------------------
///
/// Print out the usage of the Program
///
//------------------------------------------------------------------------------
static void _Usage(void)
{
  puts("\n* Get Values of Typelabel by Name*\n");
  puts("Usage: get_typelabel_value [options] <NAME1> [<NAME2> [..]]\n");
  puts("options:");
  puts("  -n print Name-Value-pair in form of NAME=VALUE");
  puts("  -v verbose (print message of unknown Value-Names");
  puts("  -a print all Typelabel-Values");
  puts("  -h print this help\n");
}


//-- Function: main ------------------------------------------------------------
///
/// The Main Function
///
/// \param argc: number of cmd-line parameter
/// \param argv: cmd-line parameter-list
///
/// \return error-code
//------------------------------------------------------------------------------
int main(int argc, char ** argv)
{
  //int i;
  tOptions opt;
  opt = _ParseOptions(argc, argv);
  if(   (   (argc <= optind)
         || (opt.usage))
     && !(opt.all))
  {
    _Usage();
    exit(EXIT_FAILURE);
  }
  tTypeLabel typelabel;
  typelabel = typelabel_OBJECT_New();
  if(TYPELABEL_RETURN_OK != typelabel_OBJECT_Open(typelabel))
  {
    puts("Open Typelabel failed!");
    exit(EXIT_FAILURE);
  }
  typelabel_OBJECT_Sync(typelabel);
  if(opt.all)
  {
    typelabel_OBJECT_PrintValueList(typelabel);
  }
  else
  {
    for(;optind<argc;optind++)
    {
      const char * name = argv[optind];
      char * value = typelabel_OBJECT_GetValue(typelabel,name);
      if(value != NULL)
      {
        if(opt.printNames)
        {
          printf("%s=",name);
          puts(value);
        }
        else if (value[0] != '\0')
        {
          puts(value);
        }
        free(value);
      }
      else if(opt.verbose)
      {
        fprintf(stderr,"error: %s not found in typelabel\n",name);
      }
    }
  }
  typelabel_OBJECT_Destroy(typelabel);
  return 0;
}

//---- End of source file ------------------------------------------------------
