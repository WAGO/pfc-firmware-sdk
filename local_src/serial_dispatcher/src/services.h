/*
 *
 *  Makefile - build and install the s2t package
 *  Copyright (C) 2017  Hans Florian Scholz
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   hans-florian.scholz@wago.com
 *
 */
#ifndef SERVICES_H_
#define SERVICES_H_
//#define _GNU_SOURCE 1
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <time.h>
typedef struct stService tService;


typedef enum {
  RESULT_INVALID = 0,
  RESULT_VALID,
  RESULT_PRIO_SUCCESS,
  RESULT_BG_SUCCESS
}tDispatchResult;

/*
 * 0: no package for this service
 * 1: package is possibly for this service
 * 2: package is complete ready for sending
 */
typedef tDispatchResult(*tCollectHandler)(struct stService * self,char byte);
typedef int(*tSendHandler)(struct stService * self);
typedef void(*tTimeoutHandler)(struct stService * self);
typedef void(*tResetHandler)(struct stService * self);
typedef short(*tGetEventsHandler)(struct stService * self);
typedef void(*tPollEventHandler)(short event,struct stService * self);
typedef void(*tReleaisHandler)(int fd,struct stService * self);

struct stService
{
    char              name[32];
    tDispatchResult   valid;
    int               fd;//fd for service to be polled (-1 for not polling )
    time_t            timeout;//timout for service in seconds
    //time_t            timeoutRest;//time remaining for timeout
    struct timespec   tsAbs;
    tCollectHandler   collect;
    tSendHandler      send;
    tTimeoutHandler   handleTimeout;
    tResetHandler     reset;
    tResetHandler     resetBackground;
    tGetEventsHandler getPollEvents;
    tPollEventHandler handleEvent;
    tReleaisHandler   relaisHandler;
    void            * serviceData;
};

typedef struct stServiceList
{
    tService             * service;
    struct stServiceList * pNext;
}tServiceList;


#define DATA_OUT     '>'
#define DATA_IN      '<'

extern int debug;

//extern time_t timeout;

//extern unsigned int cmd_port;
//extern char * cmd_host;

void printBuffer(char * buf, size_t n,char * name, char dir);
void  cerror(const char *msg, int retval);

tService * NewService(void);
tServiceList* GetServiceListElement(void);

int GetVirtualTty(void);
int getBaud(int constant);
short getPollEventsVirtualTTY(struct stService * self);

tServiceList * RegisterWAGOService(void);
tServiceList * RegisterConsoleService(void);

#endif /* SERVICES_H_ */
//---- End of source file ------------------------------------------------------
