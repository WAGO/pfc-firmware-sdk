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

#include "services.h"
#include <stdint.h>
#include <pty.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <utmp.h>




typedef struct stConsoleData {
    char  buffer[2048];
    pid_t child;
    int   actIndex;
}tConsoleData;

enum eStates
{
  STATE_START = 0,
  STATE_IDENT,
  STATE_GET_SIZE,
  STATE_COLLECT,

};

static tDispatchResult collectConsole(struct stService * self,char byte)
{
  tDispatchResult ret =RESULT_INVALID;
  tConsoleData * serviceData = (tConsoleData *)self->serviceData;

  if(self->fd == -1)
  {
      char compare[] = "console";
      if(byte == compare[serviceData->actIndex])
      {
        serviceData->actIndex++;
        ret = RESULT_VALID;
      }
      if(   (ret == 1)
         && (serviceData->actIndex >= (int)strlen(compare)))
      {
        serviceData->actIndex=0;
        ret = RESULT_BG_SUCCESS;
      }
  }
  else
  {
    serviceData->buffer[serviceData->actIndex]=byte;
    serviceData->actIndex++;
    ret = RESULT_BG_SUCCESS;
  }


  return ret;
}

static int sendConsole(struct stService * self)
{
  tConsoleData * serviceData = (tConsoleData *)self->serviceData;
  int ret = 0;
  if(self->fd == -1)
  {
    char ptsName[256];
    serviceData->child = forkpty(&(self->fd),ptsName,NULL,NULL);

    if(serviceData->child == 0)
    {
      setenv("TERM","xterm",1);
      execl("/bin/login","login",NULL);
      exit(0);
    }
  }
  else
  {
    if(debug)
    {
      printBuffer(serviceData->buffer,serviceData->actIndex, self->name, DATA_IN);
    }
    ret =  write(self->fd,serviceData->buffer,serviceData->actIndex);
  }
  return ret;
}

static void resetConsole(struct stService * self)
{
  tConsoleData * serviceData = (tConsoleData *)self->serviceData;
  serviceData->actIndex =0;
}

void stopConsole(struct stService * self)
{
  if(self->fd != -1)
  {
    tConsoleData * serviceData = (tConsoleData *)self->serviceData;
    int status;
    kill(serviceData->child,9);
    close(self->fd);
    self->fd = -1;
    waitpid(serviceData->child, &status,0);
    serviceData->child = -1;
    self->fd = -1;
    self->reset(self);
  }
}

static  void handleEventConsole(short event,struct stService * self)
{
  int closeFd = 0;
  if(event & (POLLHUP))
  {
    syslog(LOG_DEBUG, "POLLHUP");
    closeFd = 1;
  }

  if(closeFd == 1)
  {
    stopConsole(self);
  }
}

static void relaisHandlerConsole(int fd,struct stService * self)
{
  char linkPath[256];
  char fdPath[256];
  size_t szLinkPath = sizeof(linkPath);
  ssize_t linkLen;
  struct termios tios;
  speed_t ospeed;
  char baud[32];
  tcgetattr(fd, &tios);
  ospeed = cfgetospeed(&tios);
  sprintf(baud,"%d",getBaud(ospeed));
  sprintf(fdPath,"/proc/self/fd/%d",fd);
  linkLen = readlink(fdPath,linkPath,szLinkPath);
  if(0<=linkLen)
  {
    linkPath[linkLen]=0;
    while(1)
    {
      tConsoleData * serviceData = (tConsoleData *)self->serviceData;
      serviceData->child = fork();

      if(serviceData->child == 0)
      {
        execl("/sbin/getty","getty","-L",baud,linkPath,"xterm",NULL);
        exit(0);
      }
      else
      {
        int status;
        waitpid(serviceData->child,&status,0);
        serviceData->child = -1;
      }
      sleep(1);
    }
  }

}


short getPollEventsConsole(struct stService * self)
{
  (void)self;
  return POLLIN | POLLPRI | POLLRDHUP;
}

tServiceList * RegisterConsoleService(void)
{
  tServiceList * list = GetServiceListElement();
  tConsoleData * serviceData = malloc(sizeof(tConsoleData));

  serviceData->actIndex = 0;

  list->service = NewService();
  sprintf(list->service->name,"Console");
  list->service->collect = collectConsole;
  list->service->fd = -1;
  list->service->handleTimeout = stopConsole;
  list->service->getPollEvents = getPollEventsConsole;
  list->service->send = sendConsole;
  list->service->handleEvent = handleEventConsole;
  list->service->relaisHandler = relaisHandlerConsole;
  list->service->reset = resetConsole;
  list->service->resetBackground =stopConsole;
  list->service->timeout = 600000;

  list->service->serviceData = (void*)serviceData;

  return list;
}

//---- End of source file ------------------------------------------------------


//---- End of source file ------------------------------------------------------
