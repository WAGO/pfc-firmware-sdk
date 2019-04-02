//- ------------------------------------------------------------------
//-    Copyright (c) James W. Grenning -- All Rights Reserved         
//-    For use by owners of Test-Driven Development for Embedded C,   
//-    and attendees of Renaissance Software Consulting, Co. training 
//-    classes.                                                       
//-                                                                   
//-    Available at http://pragprog.com/titles/jgade/                 
//-        ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3               
//-                                                                   
//-    Authorized users may use this source code in your own          
//-    projects, however the source code may not be used to           
//-    create training material, courses, books, articles, and        
//-    the like. We make no guarantees that this source code is       
//-    fit for any purpose.                                           
//-                                                                   
//-    www.renaissancesoftware.net james@renaissancesoftware.net      
//- ------------------------------------------------------------------

#include "CppUTest/CommandLineTestRunner.h"
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <stdio.h>
#include <string.h>

static void SegementationFaultHandler(int signal)
{
   const size_t maxBacktraceSize = 100;
   void *array[maxBacktraceSize];
   size_t size;

   // get void*'s for all entries on the stack
   size = backtrace(array, maxBacktraceSize);

   // print out all the frames to stderr
   fprintf(stderr, "Error: signal %d:\n", signal);
   backtrace_symbols_fd(array, size, 2);
   exit(1);
}

bool file_exists(const char *filename)
{
  FILE *f;
  if(f = fopen(filename, "r"))
  {
    fclose(f);
    return true;
  }
  return false;
}

int main(int argc, char** argv)
{
#ifdef __UNIT_TESTS_TARGET__
   if(!file_exists("/dev/mmcblk0"))
   {
     printf("Error: no /dev/mmcblk0 found. Is an sd-card inserted?\n");
     exit(1);
   }

   if(!file_exists("/dev/mtd2"))
   {
     printf("Error: no /dev/mtd2 found. Aborting.\n");
     exit(1);
   }

   if(!file_exists("/dev/ubi0"))
   {
     printf("Error: no /dev/ubi0 found. Is the ubi device attached?\n");
     exit(1);
   }

   FILE *cmdline = fopen("/proc/cmdline", "r");
   char cmdline_buf[256];
   char *root_param;

   fread(cmdline_buf, 256, 1, cmdline);
   root_param=strstr(cmdline_buf, "root=");

   fclose(cmdline);

   if( root_param != strstr(root_param, "root=ubi"))
   {
     printf("Error: root device %s is not UBIFS. Aborting.\n", root_param);
     exit(1);
   }

#endif
   
   signal(SIGSEGV, SegementationFaultHandler);
   
   return CommandLineTestRunner::RunAllTests(argc, argv);
}

