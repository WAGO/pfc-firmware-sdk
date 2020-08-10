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


#define MAX_TELEGRAM_LENGTH  200
#define DEFAULT_PORT      6625
#define DEFAULT_HOST      "127.0.0.1"
#define DEFAULT_TIMEOUT   600

typedef struct stWAGOData {
    char  buffer[136];
    int   actIndex;
    short fullSize;
    int   state;
    struct sockaddr_in saddr;
    char host[128];
    int port;
}tWAGOData;

enum eStates
{
  STATE_START = 0,
  STATE_IDENT,
  STATE_GET_SIZE,
  STATE_COLLECT,

};

static tDispatchResult collectWAGO(struct stService * self,char byte)
{
  tDispatchResult ret =RESULT_INVALID;
  tWAGOData * serviceData = (tWAGOData *)self->serviceData;
  switch(serviceData->state)
  {
    default:
    case STATE_START:
      serviceData->buffer[0]=byte;
      serviceData->actIndex = 1;
      if(   (byte == 0xFD))
      {
        serviceData->state = STATE_IDENT;
        ret = RESULT_VALID;
      }
      else      {
        ret = RESULT_INVALID;
      }
      break;
    case STATE_IDENT:
      if(byte == 0x02)
      {
        serviceData->buffer[serviceData->actIndex]=byte;
        serviceData->actIndex++;
        serviceData->state = STATE_GET_SIZE;
        ret = RESULT_VALID;
      }
      else
      {
        ret = RESULT_INVALID;
        self->reset(self);
      }
      break;
    case STATE_GET_SIZE:
      serviceData->buffer[serviceData->actIndex]=byte;
      serviceData->actIndex++;
      ret = RESULT_VALID;
      if(serviceData->actIndex == 3)
      {
        unsigned char  * size = (unsigned char*) &(serviceData->buffer[2]);
        if(*size <= MAX_TELEGRAM_LENGTH)
        {
          serviceData->fullSize = (*size);
          serviceData->state = STATE_COLLECT;
        }
        else
        {
          ret = RESULT_INVALID;
          self->reset(self);
        }
      }
      break;
    case STATE_COLLECT:
      serviceData->buffer[serviceData->actIndex]=byte;
      serviceData->actIndex++;
      if(serviceData->actIndex >= serviceData->fullSize)
      {
        ret = RESULT_PRIO_SUCCESS;
        serviceData->state = STATE_START;
      }
      else
      {
        ret = RESULT_VALID;
      }
  }

  return ret;
}

static int initSocket(struct sockaddr_in *saddr,
                      const char *cmd_host,
                      int cmd_port)
{
  struct in_addr  cmd_hadd;
  int cmd_socket;
  int             cmd_hlen = 0;

  if (cmd_host)
  {
   struct hostent *h;

   if ( (h = gethostbyname(cmd_host)) == NULL)
   {
     syslog(LOG_ERR,"no host: %s\n", cmd_host);
     return -1;
   }
   else
   {
     cmd_hlen = h->h_length;
     memcpy(&cmd_hadd, h->h_addr, cmd_hlen);
   }
  }
  else
  {
     cmd_hadd.s_addr = INADDR_LOOPBACK;
  }

  cmd_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  saddr->sin_family = AF_INET;
  saddr->sin_port = htons (cmd_port);
  saddr->sin_addr.s_addr = cmd_hadd.s_addr;

  return cmd_socket;
}

static int sendWAGO(struct stService * self)
{
  tWAGOData * serviceData = (tWAGOData *)self->serviceData;
  if(self->fd == -1)
  {
    int ret;
    self->fd = initSocket(&(serviceData->saddr),serviceData->host,serviceData->port);
    ret = connect(self->fd,(struct sockaddr *)&(serviceData->saddr), sizeof(struct sockaddr_in));
    if (ret)
    {
      cerror("CConnect", ret);
      close(self->fd);
      self->fd = -1;
      return 0;
    }
  }
  if(debug)
  {
    printBuffer(serviceData->buffer,serviceData->actIndex, self->name, DATA_IN);
  }
  return write(self->fd,serviceData->buffer,serviceData->actIndex);
}

static void resetWAGO(struct stService * self)
{
  tWAGOData * serviceData = (tWAGOData *)self->serviceData;
  serviceData->actIndex =0;
  serviceData->state = 0;
}

static  void handleTimeoutWAGO(struct stService * self)
{
  close(self->fd);
  self->fd = -1;
  self->reset(self);
}

static  void handleEventWAGO(short event,struct stService * self)
{
  int closeSocket = 0;
  if(event & (POLLHUP))
  {
    syslog(LOG_DEBUG, "POLLHUP");
    closeSocket = 1;
  }
  if(event & (POLLERR))
  {
    syslog(LOG_DEBUG, "POLLERR");
    closeSocket = 1;
  }
  if(event & (POLLNVAL))
  {
    syslog(LOG_DEBUG, "POLLNVAL");
    closeSocket = 1;
  }
  if(event & (POLLRDHUP))
  {
    syslog(LOG_DEBUG, "POLLRDHUP");
    closeSocket = 1;
  }
  if(closeSocket == 1)
  {
    close(self->fd);
    self->fd = -1;
    self->reset(self);
  }
}

short getPollEventsWAGO(struct stService * self)
{
  (void)self;
  return POLLIN | POLLPRI | POLLRDHUP;
}

tServiceList * RegisterWAGOService(void)
{
  tServiceList * list = GetServiceListElement();
  tWAGOData * serviceData = malloc(sizeof(tWAGOData));

  serviceData->actIndex = 0;
  serviceData->port = DEFAULT_PORT;
  sprintf(serviceData->host,DEFAULT_HOST);
  memset(&(serviceData->saddr),0,sizeof(serviceData->saddr));

  list->service = NewService();
  sprintf(list->service->name,"WAGO");
  list->service->collect = collectWAGO;
  list->service->fd = -1;
  list->service->handleTimeout = handleTimeoutWAGO;
  list->service->getPollEvents = getPollEventsWAGO;
  list->service->send = sendWAGO;
  list->service->handleEvent = handleEventWAGO;
  list->service->reset = resetWAGO;
  list->service->timeout = DEFAULT_TIMEOUT;

  list->service->serviceData = (void*)serviceData;

  return list;
}

//---- End of source file ------------------------------------------------------


//---- End of source file ------------------------------------------------------
