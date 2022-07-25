//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <diagnostic/diagnostic_API.h>
#include "testapp.h"

static FILE *fpLog = NULL;
static char logFile[255] = "/tmp/diagledtest.log";
static int testLoggerActive=1;

static void _WriteLog(const char * comment)
{
  if(fpLog == NULL)
  {
    fpLog = fopen(logFile,"w");
    if(fpLog == NULL)
    {
      perror("Open logfile"),
      exit(EXIT_FAILURE);
    }
  }
  fprintf(fpLog,"%s\n",comment);
}
void TestLogger(const char * comment)
{
  if(testLoggerActive)
  {
    _WriteLog(comment);
  }
  printf("%s\n",comment);
}
void SetLogger(int onoff)
{
  testLoggerActive = onoff;
}
void LogTest(int result,const char * testName, const char * comment)
{
  char logLine[4096];
  char resultString[16];
  if(result)
  {
    sprintf(resultString,"success");
  }
  else
  {
    sprintf(resultString,"failure");
  }
  sprintf(logLine,"(%s) %s %s",resultString,testName,comment);
  _WriteLog(logLine);
  printf("%s\n",logLine);
}

int TestExit(void)
{
  return 0;
}

int YesNo(void)
{
  char buffer[8];
  printf("<y/n>: ");
  fflush(stdout);
  fflush(stdin);
  fflush(stdin);
  scanf("%s",buffer);
  if(!strcmp(buffer,"y"))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int RunTests(tTestCase * menu, size_t entries)
{
  size_t i;
  int ret = 0;
  for(i=0;i<entries;i++)
  {
    ret = menu[i].function();
    LogTest(ret,menu[i].name,"");
  }
  return 1;
}

static int _allTests(void)
{

  return 1;
}

static int _interactiveTestCases(void)
{
  return RunTests(interactiveTests,numberOfInteractiveTests);
}
static int _automaticTestCases(void)
{
  return RunTests(autoTests,numberOfAutoTests);
}

static int _selectInteractiveTest(void)
{
  tTestFunction function;
  puts("Test auswählen:?\n");
  function = selectTest(interactiveTests, numberOfInteractiveTests);
  if(function != NULL)
  {
    return function();
  }
  else
  {
    puts("fehler");
    sleep(1);
    return 1;
  }
}

static int _selectAutoTest(void)
{
  tTestFunction function;
  puts("Test auswählen:?\n");
  function = selectTest(autoTests, numberOfAutoTests);
  if(function != NULL)
  {
    return function();
  }
  else
  {
    puts("fehler");
    sleep(1);
    return 1;
  }
}

tTestCase selectMenu[] =
{
 { .name="interactive", .help="alle interaktiven tests ausführen" ,   .function=_selectInteractiveTest},
 { .name="auto",        .help="alle automatischen tests ausführen" ,  .function=_selectAutoTest},
 { .name="exit",        .help="menü verlassen" ,                      .function=TestExit},
};

static int _selectTest(void)
{
  tTestFunction function;
  puts("Aus Welcher soll der test ausgewählt werden?\n");
  function = selectTest(selectMenu, sizeof(selectMenu)/sizeof(tTestCase));
  if(function != NULL)
  {
    return function();
  }
  else
  {
    puts("fehler");
    sleep(1);
    return 1;
  }
}



tTestCase mainMenu[] =
{
 { .name="all",         .help="alle tests ausführen" ,                .function=_allTests},
 { .name="interactive", .help="alle interaktiven tests ausführen" ,   .function=_interactiveTestCases},
 { .name="auto",        .help="alle automatischen tests ausführen" ,  .function=_automaticTestCases},
 { .name="nonstop",     .help="dauertest starten" ,                   .function=NULL},
 { .name="select",      .help="einen test geziehlt auswählen" ,       .function=_selectTest},
 { .name="exit",        .help="programm beenden" ,                    .function=TestExit},
};

static void printMenu(tTestCase * menu, size_t entries)
{
  size_t i;
  //size_t entries=sizeof(mainMenu)/sizeof(tTestCase);
  int maxNameLen=0;
  for(i=0;i<entries;i++)
  {
    int actLen=strlen(menu[i].name);
    if(actLen > maxNameLen)
    {
      maxNameLen = actLen;
    }
  }
  maxNameLen += 2;
  for(i=0;i<entries;i++)
  {
    char buffer[2048];
    int j;
    for(j=0;j<2048;j++)
    {
      buffer[j]=' ';
    }
    buffer[2047]=0;
    sprintf(buffer,"%s:",menu[i].name);
    buffer[strlen(buffer)]=' ';
    sprintf(&buffer[maxNameLen],"%s",menu[i].help);
    printf("%s\n",buffer);
  }
}

tTestFunction  selectTest(tTestCase * menu, size_t entries)
{
  char buffer[255];
  tTestFunction  ret = NULL;

  printMenu(menu,entries);
  printf("\ninput: ");
  fflush(stdout);
  fflush(stdin);
  fflush(stdin);
  scanf("%s",buffer);
  size_t i;
  for(i=0;i<entries;i++)
  {
    if(!strcmp(menu[i].name,buffer))
    {
      ret = menu[i].function;
      break;
    }
  }
  return ret;
}

int main(void)
{
  int run = 1;
  while(run)
  {
    tTestFunction  function;

    log_EVENT_Init("ledTest");

    puts("Diagnostic LED Testapp");
    puts("Bitte sicherstellen:");
    puts(" - der LED-Server läuft");
    puts(" - keine anderen Programme nehmen einfluss auf die LEDs");
    puts(" - /tmp/led.xml ist valide");
    puts(" - /etc/specific/diagnostic.xml ist unter anderem namen gesichert");
    puts("testmodus auswählen:");
    function = selectTest(mainMenu, sizeof(mainMenu)/sizeof(tTestCase));
    if(function != NULL)
    {
      run = function();
    }
    else
    {
      puts("Fehlerhafte Eingabe\n");
      sleep(3);
    }
    system("clear");

  }
  return 0;
}
