/*
 *
 *  Makefile - build and install the s2t package
 *  Copyright (C) 2007  Dieter Etz
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
 *   dieter@13thfloor.at
 *
 */



#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <termios.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <pty.h>
#include <limits.h>

#include "services.h"

#define DEFAULT_DEVICE    "/dev/ttyS0"
#define DEFAULT_BAUDRATE  19200
#define DEFAULT_BAUDRATE_C  B19200

#define POLL_FD_SERIAL    0
#define POLL_FD_TCP       1

#define LOGBUFFER_SIZE 1024
#define PRINTBUFFER_SIZE 8192



typedef struct stBaudrate {
    int baudConstant;
    int baudNumber;
}tBaudrate;

tServiceList * serviceRoot = NULL;

const tBaudrate baudrates[] = {{B230400, 230400},
                               {B115200, 115200},
                               {B57600,  57600},
                               {B38400,  38400},
                               {B19200,  19200},
                               {B9600,   9600},
                               {B4800,   4800},
                               {B2400,   2400},
                               {B1800,   1800},
                               {B1200,   1200},
                               {B600,    600},
                               {B200,    200},
                               {B150,    150},
                               {B134,    134},
                               {B110,    110},
                               {B75,     75},
                               {B50,     50},
                               {B0,      0}};

#define OPTIONS       "hb:p:d:v:a:lwc:V"

static char *          cmd_name;
struct termios        oldtio;
int                   serfd = -1;
int                   virtfd = -1;
//time_t                timeout = 10000;
//unsigned int    cmd_port = DEFAULT_PORT;
//unsigned int    cmd_backlog = 2;
//char *          cmd_host = NULL;
char *          cmd_device = DEFAULT_DEVICE;
char *          cmd_virtual = "/dev/ttyRUNTIME1000";
char *          cmd_alternate = "/dev/ttyRUNTIME1";
int             cmd_socket = 0;
int             cmd_baud   = DEFAULT_BAUDRATE;
int             cmd_parity  = 2;
int             debug = 0;
int             cmd_console = 0;
int             cmd_wago = 0;


void  cerror(const char *msg, int retval)
{
  syslog(LOG_ERR, "%s: [%s,%d,%d] %s",
         cmd_name, msg, retval, errno,
         strerror(errno));
  return;
}

#define MAX_LINE    2048

#define MAX(a,b)    (((a)>(b))?(a):(b))

void CloseSerial(int fd, struct termios *termIOs)
{
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW, termIOs);
  close(fd);
}
void ResetChilds(void)
{
  tServiceList * pAct = serviceRoot;
  while(pAct)
  {
    if(pAct->service->resetBackground != NULL)
    {
      pAct->service->resetBackground(pAct->service);
    }
    pAct = pAct->pNext;
  }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void SigHandler(int sig, siginfo_t *info, void *data)
{
  syslog(LOG_ERR,"received signal %d: %m",sig);
  close(virtfd);
  CloseSerial(serfd, &oldtio);
  ResetChilds();
  exit(EXIT_FAILURE);
}
#pragma GCC diagnostic warning "-Wunused-parameter"

void logBuffer(char * buf, int n, char IO)
{
  char logstr[LOGBUFFER_SIZE];
  int remaining = LOGBUFFER_SIZE;
  int offset = 0;
  int i;
  logstr[0] = '\0';
  syslog(LOG_DEBUG, "Got %d bytes: RS232 %c TCP", n , IO);

  for(i = 0; i < n; i++ )
  {
    int len = snprintf(&logstr[offset], remaining, "%.2x", (buf[i] & 0xff));
    if (len >= remaining)
    {
      break;
    }
    offset += len;
    remaining -= len;
  }
  syslog(LOG_DEBUG, "%s", logstr);

}

void printBuffer(char * buf, size_t n,char * name, char dir)
{
  char logstr[PRINTBUFFER_SIZE];
  size_t i,j;
  int offset = snprintf(logstr, PRINTBUFFER_SIZE, "%.4zu %s %c ", n, name, dir);
  int remaining = PRINTBUFFER_SIZE - offset;
  j = 0;
  for(i = 0; i < n; i++)
  {
    if(++j > 10)
    {
      int len = snprintf(&logstr[offset], remaining, "\n       ");
      if (len >= remaining)
      {
        break;
      }
      offset += len;
      remaining -= len;
      j=1;
    }
    int len = snprintf(&logstr[offset], remaining, " %.2X", (buf[i] & 0xff));
    if (len >= remaining)
    {
      break;
    }
    offset += len;
    remaining -= len;
  }
  puts(logstr);
}

struct pollfd * updatePollFds(struct pollfd * fds,size_t * anzFds,size_t * fdsSize ,tServiceList * pAct)
{
  while(pAct != NULL)
  {
    size_t i;
    int found = 1;
    for(i = 0; i < *anzFds;i++)
    {
      if(   (pAct->service->fd < 0)
         || (pAct->service->fd == fds[i].fd))
      {
        found = 0;
        break;
      }
    }
    if(found)
    {
      if(*fdsSize <= *anzFds)
      {
        (*fdsSize)++;
        fds = realloc(fds,(*fdsSize) * sizeof (struct pollfd));
      }
      fds[*anzFds].fd = pAct->service->fd;
      fds[*anzFds].events = pAct->service->getPollEvents(pAct->service);
      (*anzFds)++;
    }
    pAct = pAct->pNext;
  }

  return fds;
}

void resetTimeout(time_t timeout, struct timespec * tsAbs)
{
  clock_gettime(CLOCK_MONOTONIC,tsAbs);

  tsAbs->tv_sec += timeout;
}

void updateTimeouts(int fd,tServiceList * pAct)
{
  while(pAct != NULL)
  {
    if(pAct->service->fd == fd)
    {
      resetTimeout(pAct->service->timeout, &pAct->service->tsAbs);
    }
    pAct = pAct->pNext;
  }

}
//result = val1 - val2
void tsDiff(struct timespec * result,struct timespec * val1, struct timespec * val2)
{
  result->tv_sec = val1->tv_sec - val2->tv_sec;
  if(val1->tv_nsec < val2->tv_nsec)
  {
    result->tv_sec--;
    result->tv_nsec =  val1->tv_nsec + 1000000000 - val2->tv_nsec;
  }
  else
  {
    result->tv_nsec =  val1->tv_nsec - val2->tv_nsec;
  }
}

struct timespec * setTo(struct timespec * absTo,struct timespec * buffer, struct timespec * now)
{
    struct timespec * ret = NULL;

    if(   (   (absTo->tv_sec > now->tv_sec)
           || ((absTo->tv_sec == now->tv_sec) && (absTo->tv_nsec > (now->tv_nsec+1000000))))
       && (   (buffer->tv_sec > absTo->tv_sec)
           || ((buffer->tv_sec == absTo->tv_sec) && (buffer->tv_nsec > (absTo->tv_nsec)))))
    {
      ret = buffer;

      tsDiff(ret,absTo, now);
    }

    return ret;
}

int PollAll(struct pollfd *fds, nfds_t anzFds,tServiceList * services,struct timespec * diff)
{
  struct timespec start;
  struct timespec stop, to={INT_MAX,INT_MAX}, *pTo = NULL;
  int polled;
  tServiceList * pAct = services;

  clock_gettime(CLOCK_MONOTONIC,&start);

  while(pAct != NULL)
  {
    struct timespec * pTmp;
    pTmp = setTo(&(pAct->service->tsAbs),&to, &start);
    if(pTmp != NULL)
    {
      pTo = pTmp;
    }
    pAct = pAct->pNext;
  }

  //polled = poll(fds,anzFds,timeout);
  polled = ppoll(fds,anzFds,pTo,NULL);

  clock_gettime(CLOCK_MONOTONIC,&stop);

  tsDiff(diff,&stop, &start);

  return polled;

}

void resetAll(tServiceList * pAct)
{
  while(pAct != NULL)
  {
    pAct->service->valid = RESULT_VALID;
    pAct->service->reset(pAct->service);
    pAct = pAct->pNext;
  }
}

static void CallTimeouts(tServiceList * pAct)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC,&now);
  while(pAct != NULL)
  {
    struct timespec diff;
    tsDiff(&diff,&pAct->service->tsAbs, &now);
    if(   (diff.tv_sec < 0)
       || ((diff.tv_sec == 0) && (diff.tv_nsec < 0)))
    {

      if(pAct->service->handleTimeout != NULL)
      {
        pAct->service->handleTimeout(pAct->service);
      }
    }
    pAct = pAct->pNext;
  }
}

static void HandleEvents(short event, int fd,tServiceList * pAct)
{
  while(pAct != NULL)
  {
    if(fd == pAct->service->fd)
    {
      if(pAct->service->handleEvent != NULL)
      {
        pAct->service->handleEvent(event,pAct->service);
      }
    }
    pAct = pAct->pNext;
  }
}

void resetBackgrounds(tServiceList * services)
{
  tServiceList * pAct = services;
  while(pAct != NULL)
  {
    if(pAct->service->resetBackground != NULL)
    {
      pAct->service->resetBackground(pAct->service);
    }
    pAct = pAct->pNext;
  }
}

int CollectAndSend(char byte,tServiceList * services)
{
  int ret = 0;
  int bg = 0;
  tServiceList * pAct = services;
  while(pAct != NULL)
  {
    if(pAct->service->valid != RESULT_INVALID)
    {
      tDispatchResult result;
      result = pAct->service->collect(pAct->service,byte);
      pAct->service->valid = result;
      if(result == RESULT_PRIO_SUCCESS)
      {
        ret = 1;
        pAct->service->send(pAct->service);
        resetBackgrounds(services);
        resetTimeout(pAct->service->timeout, &(pAct->service->tsAbs));
        break;
      }
      else if(result == RESULT_BG_SUCCESS)
      {
        bg = 1;
      }
    }
    pAct = pAct->pNext;
  }

  if(bg && !ret)
  {
    int valids = 0;
    pAct = services;
    while(pAct != NULL)
    {
      if(pAct->service->valid == RESULT_VALID)
      {
        valids = 1;
        break;
      }
      pAct = pAct->pNext;
    }
    if(!valids)
    {
      pAct = services;
      while(pAct != NULL)
      {
        if(pAct->service->valid == RESULT_BG_SUCCESS)
        {
          ret = 1;
          pAct->service->send(pAct->service);
          resetTimeout(pAct->service->timeout, &(pAct->service->tsAbs));
        }
        pAct = pAct->pNext;
      }
    }
  }

  return ret;
}

int CheckValids(tServiceList * pAct)
{
  int ret = 0;
  while(pAct != NULL)
  {
    if(pAct->service->valid != RESULT_INVALID)
    {
      ret = 1;
      break;
    }
    pAct = pAct->pNext;
  }
  return ret;
}

static void Dispatch(int fdTty, tServiceList * services)
{
  struct pollfd * fds = NULL;
  size_t         fdsSize = 1;
  size_t         anzFds  = 1;

  fds = realloc(fds,fdsSize * sizeof (struct pollfd));

  resetAll(services);
  while(1)
  {
    int polled;
    struct timespec neededTime;
    fds[0].fd = fdTty;
    fds[0].events = POLLIN;
    anzFds=1;
    fds = updatePollFds(fds,&anzFds,&fdsSize ,services);
    polled = PollAll(fds,anzFds,services,&neededTime);
    if(neededTime.tv_sec > 0)
    {
      if(debug)
      {
        printf("reset because of input data timout!!!\n");
      }
      resetAll(services);
    }
    if(polled == 0)
    {
      CallTimeouts(services);
    }
    else if(polled > 0)
    {
      int i;
      int bytes;
      char buffer[MAX_LINE];

      //read service FDs
      for(i = anzFds-1; i > 0;i--)
      {
        if(fds[i].revents == 0)
        {
          continue;
        }
        if(fds[i].revents & ~POLLIN)
        {
          HandleEvents(fds[i].revents,fds[i].fd, services);
        }
        else if(fds[i].revents & POLLIN)
        {
          bytes = read(fds[i].fd, buffer, MAX_LINE);
          if(debug)
          {
            char name[32];
            sprintf(name, "%d",fds[i].fd);
            printBuffer(buffer, bytes, name, DATA_OUT);
          }
          if(bytes > 0)
          {
            write(fdTty, buffer,bytes);
          }
          updateTimeouts(fds[i].fd,services);
        }

      }

      if(fds[0].revents & POLLIN)
      {
        int i;
        bytes = read(fds[0].fd, buffer, MAX_LINE);
        if(debug)
        {
          printBuffer(buffer, bytes, "read", DATA_IN);
        }
        for(i=0;i<bytes;i++)
        {
          if(CollectAndSend(buffer[i],services))
          {
            resetAll(services);
          }
        }
      }
      if(!CheckValids(services))
      {
        if(debug)
        {
          printf("no service valid. unhandled Data!!!\n");
        }
        resetAll(services);
      }
    }
  }
}

int getBaud(int constant)
{
  int i;
  int ret = 0;
  for(i = 0; baudrates[i].baudConstant > 0 ; i++)
  {
    if(baudrates[i].baudConstant == constant)
    {
      ret = baudrates[i].baudNumber;
      break;
    }
  }
  return ret;
}

int selectBaud(int baud)
{
  int i;
  int ret = DEFAULT_BAUDRATE_C;
  for(i = 0; baudrates[i].baudNumber > 0 ; i++)
  {
    if(baudrates[i].baudNumber == baud)
    {
      ret = baudrates[i].baudConstant;
      break;
    }
  }
  return ret;
}

static int initPhysicalSerialDevice(const char *device, int baudrate)
{
  int serfd;
  struct termios newtio;
  serfd = open (device, O_RDWR | O_NOCTTY );
  if (serfd < 0)
  {
    cerror("DOpen", serfd);
    exit(3);
  }

  tcgetattr(serfd,&oldtio);

  bzero(&newtio, sizeof(newtio));

  //activate parity EVEN but ignore result
  newtio.c_cflag = baudrate | CS8 | CLOCAL | CREAD;// | PARENB;
  if(cmd_parity == 2)
  {
    newtio.c_cflag |= PARENB;
    newtio.c_cflag &= ~PARODD;
  }
  else if (cmd_parity == 1)
  {
    newtio.c_cflag |= (PARENB | PARODD);
  }
  else
  {
    newtio.c_cflag &= ~(PARENB | PARODD);
  }
  newtio.c_cflag &= ~PARODD;

  newtio.c_iflag &= ~(INPCK|PARMRK|IGNPAR);
  newtio.c_iflag |= ICANON;

  newtio.c_oflag = 0;
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;
  newtio.c_cc[VMIN]     = 1;

  tcflush(serfd, TCIFLUSH);
  tcsetattr(serfd,TCSANOW,&newtio);

  return serfd;
}

static int initVirtualSerialDevice(const char * cmd_virtual)
{
  int fdMaster, fdSlave;
  struct termios newtio;
  char   ptyName[1024];

  bzero(&newtio, sizeof(newtio));

  newtio.c_cflag = B4000000 | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = 0;
  newtio.c_oflag = 0;


  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;
  newtio.c_cc[VMIN]     = 1;

  if(0 > openpty(&fdMaster,&fdSlave,ptyName,&newtio,NULL))
  {
    syslog(LOG_ERR, "Open PTY failed\n");
    return -1;
  }

  //close(fdSlave);

  printf("pty was opend: \"%s\"\n",ptyName);
  unlink(cmd_virtual);
  if(0 > symlink(ptyName, cmd_virtual))
  {
    syslog(LOG_ERR, "Creating symlink PTY failed\n");
    return -1;
  }

  return fdMaster;
}

tServiceList* GetServiceListElement(void)
{
  tServiceList * pNew = malloc(sizeof(tServiceList));

  pNew->pNext = NULL;
  pNew->service = NULL;

  return pNew;
}

tService * NewService(void)
{
  tService * ret = malloc(sizeof(tService));

  ret->collect = NULL;
  ret->handleTimeout = NULL;
  ret->getPollEvents = NULL;
  ret->handleEvent = NULL;
  ret->relaisHandler = NULL;
  ret->send = NULL;
  ret->serviceData = NULL;
  ret->reset = NULL;
  ret->resetBackground = NULL;
  ret->timeout = -1;
  ret->tsAbs.tv_sec = 0;
  ret->tsAbs.tv_nsec = 0;
  ret->fd = -1;
  ret->valid = RESULT_INVALID;

  return ret;
}

int GetVirtualTty(void)
{
  static int once=0;
  if(!once)
  {
    virtfd = initVirtualSerialDevice(cmd_virtual);
    once =1;
  }
  return virtfd;
}

short getPollEventsVirtualTTY(struct stService * self)
{
  (void)self;
  return POLLIN;
}

tServiceList * AddService(tServiceList * serviceList,tServiceList * newServices)
{

  if(serviceList == NULL)
  {
    serviceList = newServices;
  }
  else
  {
    tServiceList * pAct = serviceList;
    while(pAct->pNext != NULL)
    {
      pAct = pAct->pNext;
    }
    pAct->pNext = newServices;

  }

  return serviceList;
}

static tServiceList * RegisterServices(tServiceList * serviceList)
{
  if(cmd_wago)
  {
    serviceList = AddService(serviceList,RegisterWAGOService());
  }

  if(cmd_console)
  {
    serviceList = AddService(serviceList,RegisterConsoleService());
  }


  return serviceList;
}

int main(int argc, char *argv[])
{
  int            c, errflg = 0;
  struct sigaction signals;
  int i;



  cmd_name = argv[0];
  openlog("ttydispatcherd", LOG_NDELAY | LOG_CONS | LOG_PID ,LOG_LOCAL0);

  while ((c = getopt(argc, argv, OPTIONS)) != EOF)
  {
    switch (c) {
      case 'h':
        fprintf(stderr,
        "This is %s " VERSION "\n"
        "options are:\n"
        "-h          print this help message\n"
        "-d <dev>    device (default: /dev/ttyS0)\n"
        "-b <baud>   used baudrate\n"
        "-p <parity> set Parity"
        "-v <name>   name of virtual tty\n"
        "-a <name>   name of alternate tty link name if no service is selected"
        "-l          activate login-shell (write \"console\" for startup)\n"
        "-w          activate wago-protocol\n"
        "-V          verbose\n",
        cmd_name);
        exit(0);
        break;
      case 'b':
        cmd_baud = atol(optarg);
        break;
      case 'p':
        cmd_parity = atol(optarg);
        break;
      case 'd':
        cmd_device = optarg;
        break;
      case 'v':
        cmd_virtual = optarg;
        break;
      case 'a':
        cmd_alternate = optarg;
        break;
      case 'l':
        cmd_console=1;
        break;
      case 'w':
        cmd_wago=1;
        break;
      case 'V':
        debug = 1;
        break;
      case '?':
      default:
        errflg++;
        break;
      }
  }

  if (errflg)
  {
    syslog(LOG_ERR,
          "Usage: %s -[" OPTIONS "]\n"
          "%s -h for help.\n",
          cmd_name, cmd_name);
      exit(2);
  }


  signals.sa_sigaction = SigHandler;
  sigemptyset(&signals.sa_mask);
  signals.sa_flags = 0;
  for(i = 1; i < SIGRTMAX; i++)
  {
    if(   (i != 9)
       && (i != 17)//SIGCHLD
       && (i != 19)
       && (i != 32)
       && (i != 33))
    if(sigaction(i, &signals, NULL))
    {
      syslog(LOG_ERR,"could not init signal %d: %m", i);
    }
  }
  //syslog(LOG_ERR, "Wait for serial and then connect to %s:%d.\n",  cmd_host, cmd_port);


  serviceRoot = RegisterServices(serviceRoot);

  if(serviceRoot != NULL)
  {
    int count = 0;
    tServiceList * pAct = serviceRoot;
    while(pAct != NULL)
    {
      count++;
      pAct = pAct->pNext;
    }
    serfd = initPhysicalSerialDevice(cmd_device,selectBaud(cmd_baud));
    if(count>1)
    {
      Dispatch(serfd,serviceRoot);
    }
    else
    {
      if(serviceRoot->service->relaisHandler != NULL)
      {
        serviceRoot->service->relaisHandler(serfd,serviceRoot->service);
      }
      else
      {
        Dispatch(serfd,serviceRoot);
      }
    }
  }
  else
  {
    syslog(LOG_INFO,"no service selected create link  from %s to %s",cmd_device,cmd_virtual);
    if(0 > symlink(cmd_device, cmd_alternate))
    {
      syslog(LOG_ERR, "Creating symlink PTY failed\n");
      return -1;
    }
  }


  close(cmd_socket);
  CloseSerial(serfd, &oldtio);
  exit(0);
}
