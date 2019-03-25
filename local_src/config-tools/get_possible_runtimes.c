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
///  \file     get_possible_runtimes.c
///
///  \version  $Rev: 17623 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <typelabel_API.h>

#define CUSTOM   0x01
#define CODESYS2 0x02
#define CODESYS3 0x04

static const char * custom = "0";
static const char * codesys2 = "2";
static const char * codesys3 = "3";
static const char * true = "true";
static const char * false = "false";

typedef struct {
  uint8_t json;
  uint8_t help;
  uint32_t runtime;
}tOptions;

static const char optlist[] = "e3c2u0jh";

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
      case 'u':
      case '0':
        if(ret.runtime)
        {
          ret.help = 1;
        }
        ret.runtime = CUSTOM;
        break;
      case 'c':
      case '2':
          if(ret.runtime)
          {
            ret.help = 1;
          }
          ret.runtime = CODESYS2;
          break;
      case 'e':
      case '3':
          if(ret.runtime)
          {
            ret.help = 1;
          }
          ret.runtime = CODESYS3;
          break;
      case 'j':
          ret.json = 1;
          break;
      case 'h':
          ret.help = 1;
          break;
      case '?':
      default:
          fprintf(stderr,"unknown arg %c\n", optopt);
          ret.help =1;
          break;
    }
  }

  return ret;
}

static void _Usage(void)
{
  puts("\n* get Possible Runtimes for this device*\n");
  puts("Usage: get_possible_runtimes [options]\n");
  puts("Calling without options returns all possible Runtimes");
  puts("In this case retuen values are:\n0 (custom); 2 (codesys2) 3 (eruntime)\n");
  puts("The Options -u, -0,-c, -2, -e and -3 are exclusive and result in true or false.\n");
  puts("options:");
  puts("  -j       print in json format");
  puts("  -u or -0 is custom runtime posible");
  puts("  -c or -2 is codesys2 possible");
  puts("  -e or -3 is eruntime posible\n");
}

void jsonTrueFalse(const char * start,const char * key,uint32_t trueFalse,const char * end)
{
  printf("%s\"%s\":\"",start,key);
  if(trueFalse)
  {
    printf(true);
  }
  else
  {
    printf(false);
  }
  printf("\"%s",end);
}

static void printTrueFalse(uint32_t runtime,uint32_t runtimeBits,uint8_t json)
{
  int result=(runtimeBits & runtime);

  if(json)
  {
    char * pRuntime = NULL;
    switch(runtime)
    {
      case CUSTOM:
        pRuntime = custom;
        break;
      case CODESYS2:
        pRuntime = codesys2;
        break;
      case CODESYS3:
        pRuntime = codesys3;
        break;
    }
    jsonTrueFalse("{",pRuntime,(runtimeBits & runtime),"}");
  }
  else
  {
    if((runtimeBits & runtime))
    {
      printf(true);
    }
    else
    {
      printf(false);
    }
    printf("\n");
  }
}

static void printRuntimes(uint32_t runtimeBits,uint8_t json)
{
  char   space[]=" ";
  char   komma[]=",";
  char * pDelimitter = space;

  int havePrinted=0;
  if(json)
  {

  }
  if(json)
  {
  /*  jsonTrueFalse("{",custom,(runtimeBits & CUSTOM),",");
    jsonTrueFalse("",codesys2,(runtimeBits & CODESYS2),",");
    jsonTrueFalse("",codesys3,(runtimeBits & CODESYS3),"}");*/
    printf("{\"availableVersions\":[");
    pDelimitter=komma;

  }
    if(runtimeBits & CUSTOM)
    {
      printf("%s",custom);
      havePrinted=1;
    }
    if(runtimeBits & CODESYS2)
    {
      if(havePrinted)
      {
        printf(pDelimitter);
      }
      printf("%s",codesys2);
      havePrinted=1;
    }
    if(runtimeBits & CODESYS3)
    {
      if(havePrinted)
      {
        printf(pDelimitter);
      }
      printf("%s",codesys3);
      havePrinted=1;

    }
    if(!json && havePrinted)
    {
      printf("\n");
    }
    if(json)
    {
      printf("]}");
    }
}

static uint32_t checkBlacklist(char * value)
{
  uint32_t blacklist = 0;
  //char * lineptr = NULL;
  static int once = 0;
  static char ** list = NULL;
  static int count = 0;
  int i;

  if(once == 0)
  {
    FILE * fp = fopen("/etc/specific/rtsbl","r");
    if(fp != NULL)
    {
      long fsize;
      char *string;
      char * line;
      fseek(fp, 0, SEEK_END);
      fsize = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      string = malloc(fsize + 1);
      fread(string, fsize, 1, fp);
      fclose(fp);
      string[fsize] = 0;


      line = strtok(string,"\n");
      while(line != NULL)
      {
        for(i=0;line[i]==' ' && line[i]!=0;i++);
        if(line[i] != '#' && line[i] != 0)
        {
          int k = strlen(line)-1;
          for(;k>i && line[k] == ' ';k--);
          line[k+1]=0;

          list = realloc(list,sizeof(char*)*(count+1));
          list[count] = strdup(&line[i]);
          count++;
        }

        line = strtok(NULL,"\n");
      }

      free(string);

      once = 1;
    }
  }

  for(i=0;i<count;i++)
  {
    if(!strcmp(value,list[i]))
    {
      blacklist=1;
      break;
    }
  }

  return blacklist;
}

int main(int argc,char *argv[])
{
  tTypeLabel typelabel;
  char * cds2 = NULL;
  char * cds3 = NULL;
  uint32_t runtimeBits=CUSTOM;
  tOptions opt;
  opt = _ParseOptions(argc, argv);
  if(opt.help)
  {
    _Usage();
    exit(EXIT_FAILURE);
  }

  typelabel = typelabel_OBJECT_New();
  if(TYPELABEL_RETURN_OK != typelabel_OBJECT_Open(typelabel))
  {
    puts("Open Typelabel failed!");
    exit(EXIT_FAILURE);
  }
  typelabel_OBJECT_Sync(typelabel);
  cds2 = typelabel_OBJECT_GetValue(typelabel,"TARGETID");
  cds3 = typelabel_OBJECT_GetValue(typelabel,"DEVICEID");
  typelabel_OBJECT_Destroy(typelabel);

  if(   (cds2 != NULL)
     && (0 != strtoul(cds2,NULL,0))
     && (0 == checkBlacklist(cds2)))
  {
    runtimeBits|=CODESYS2;
    free(cds2);
  }

  if(   (cds3 != NULL)
     && (0 != strtoul(cds3,NULL,0))
     && (0 == checkBlacklist(cds3)))
  {
    runtimeBits|=CODESYS3;
    free(cds3);
  }

  if(opt.runtime)
  {
    printTrueFalse(opt.runtime,runtimeBits,opt.json);
  }
  else
  {
    printRuntimes(runtimeBits,opt.json);
  }

  return 0;
}

//---- End of source file ------------------------------------------------------
