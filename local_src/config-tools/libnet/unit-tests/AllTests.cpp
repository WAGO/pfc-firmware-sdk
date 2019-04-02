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
    if( (f = fopen(filename, "r")) )
    {
        fclose(f);
        return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    signal(SIGSEGV, SegementationFaultHandler);

    return CommandLineTestRunner::RunAllTests(argc, argv);
}

