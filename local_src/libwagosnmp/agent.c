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
///  \file     server.c
///
///  \version  $Rev: 15077 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "wagosnmp_API.h"
#include "wagosnmp_internal.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

typedef struct stHandlerList tHandlerList;

struct stHandlerList {
    netsnmp_handler_registration * handler;
    tHandlerList * pNext;
};

static tHandlerList * pHandlerListRoot = NULL;
static pthread_t stThreadID = 0;
static mqd_t trap_queue = -1;
static sem_t* oidMutex = NULL;
static int    oidShmFd = -1;
static tOidShm * pOidShm = NULL;
static int   localShmSize = sizeof(tOidShm);

int __attribute__((weak)) netsnmp_unregister_handler(netsnmp_handler_registration *reginfo)
{
  (void) reginfo;
  return 1;
}

netsnmp_handler_registration * __attribute__((weak)) netsnmp_create_handler_registration(const char *name,
                                                                                         Netsnmp_Node_Handler* handler_access_method,
                                                                                          const oid *reg_oid, size_t reg_oid_len,
                                                                                          int modes)
{
  (void)name;
  (void)handler_access_method;
  (void)reg_oid;
  (void)reg_oid_len;
  (void)modes;
  return NULL;
}

int __attribute__((weak)) netsnmp_register_instance(netsnmp_handler_registration * reginfo)
{
  (void) reginfo;
  return 1;
}

int __attribute__((weak)) netsnmp_set_request_error(netsnmp_agent_request_info  *reqinfo,
                                                    netsnmp_request_info *request, int error_value)
{
  (void)reqinfo;
  (void)request;
  (void)error_value;
  return 1;
}

int __attribute__((weak)) netsnmp_send_traps(int trap, int specific,
                                                    const oid * enterprise, int enterprise_length,
                                                    netsnmp_variable_list * vars,
                                                    const char * context, int flags)
{
  (void)trap;
  (void)specific;
  (void)enterprise;
  (void)enterprise_length;
  (void)vars;
  (void)context;
  (void)flags;
  return 1;
}
void __attribute__((weak)) send_easy_trap(int a, int b)
{
  (void)a;
  (void)b;
}

static void _TrapSend(tWagoSnmpMsg * trap)
{
  netsnmp_variable_list *notification_vars = NULL;


  if(trap->variable.type != ASN_NULL)
  {
    snmp_varlist_add_variable(&notification_vars,
                              trap->variable.sOID,  trap->variable.sOID_length,
                              trap->variable.type,
                              (u_char *) trap->variable.buf,trap->variable.len);
  }
  netsnmp_send_traps  (trap->trap_type,trap->specific_type,trap->enterprise,trap->enterprise_length,
                     notification_vars, NULL,0);
  snmp_free_varbind(notification_vars);
}

static int _OidHandler(netsnmp_mib_handler *handler, netsnmp_handler_registration *reginfo,
                                   netsnmp_agent_request_info *reqinfo, netsnmp_request_info *requests)
{
  (void)(handler);
  (void)(reginfo);
  int ret=SNMP_ERR_NOERROR;
  tOidObject * object = NULL;

  /*build_oid_string(szOID, requests->requestvb->name, requests->requestvb->name_length);
  pagent_oid = find_agent_oid(szOID);
  */
  AGENT_MutexLock();
  object = AGENT_GetOidObject(requests->requestvb->name,requests->requestvb->name_length);
  if(object != NULL)
  {
    switch (reqinfo->mode)
    {
      case MODE_GET:
      case MODE_GETNEXT:
        snmp_set_var_typed_value(requests->requestvb, object->type, object->buf, object->len);

        break;
      case MODE_SET_RESERVE1:
        // check type
        if(object->type != requests->requestvb->type)
        {
          netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGTYPE);
          ret = SNMP_ERR_WRONGTYPE;
        }

        break;
      case MODE_SET_RESERVE2:
        // no storage for old value -> no action
        break;
      case MODE_SET_ACTION:
        // update current value
        if(object->readOnly == 0)
        {
          AGENT_SetOidObjectValue(object,requests->requestvb);
        }
        else
        {
          netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_NOTWRITABLE);
        }
        break;
      case MODE_SET_UNDO:
        // restore information -> no action
        break;
      case MODE_SET_COMMIT:
        // commit set value -> no action
        break;
      case MODE_SET_FREE:
        // free set value -> no action
        break;
    }
  }
  else
  {
    // error
    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
  }
  AGENT_MutexUnlock();
  return ret;
}

static void _AddHandlerToList(netsnmp_handler_registration *new_handler)
{

  tHandlerList * pNew = malloc(sizeof(tHandlerList));
  pNew->handler = new_handler;
  pNew->pNext = NULL;
  if(pHandlerListRoot == NULL)
  {
    pHandlerListRoot = pNew;
  }
  else
  {
    tHandlerList * pAct = pHandlerListRoot;
    while(pAct->pNext != NULL)
    {
      pAct = pAct->pNext;
    }
    pAct->pNext = pNew;
  }
}

static void _RegisterOIDHandler(oid * anOID, size_t anOID_length)
{
  netsnmp_handler_registration *new_handler = NULL;
  new_handler = netsnmp_create_handler_registration(WAGOSNMP_AGENT_NAME,_OidHandler, \
                anOID,anOID_length,HANDLER_CAN_RWRITE);
  if(new_handler)
  {

    // register handler
    /*iTmpRet =*/ netsnmp_register_instance(new_handler);
    _AddHandlerToList(new_handler);
  }
}

static void _RegisterOID(tWagoSnmpMsg * msg)
{

  AGENT_CreateShm();
  AGENT_RemapShm();
  AGENT_MutexLock();
  if(oidShmFd >= 0)
  {
    tOidObject * pAct = AGENT_GetOidObject(msg->variable.sOID, msg->variable.sOID_length);
    if(pAct != NULL)
    {
      _RegisterOIDHandler(pAct->anOID,pAct->anOID_length);
    }

  }
  AGENT_MutexUnlock();
}

static void _Reset(void)
{
  tHandlerList * pAct = pHandlerListRoot;

  while(pAct != NULL)
  {
    tHandlerList * del = pAct;
    netsnmp_unregister_handler(del->handler);
    pAct = pAct->pNext;
    free(del);
  }
  pHandlerListRoot = NULL;
  AGENT_CloseMutex();
  AGENT_CloseShm();
}

static void* _ServerMain()
{
  while(1)
    {
      struct pollfd fdrec;
      fdrec.fd = trap_queue;
      fdrec.events = POLLIN;
      fdrec.revents = 0;
      // wait for message (forever) used because we opened fd in O_NONBLOCK mode
      if(0 < poll(&fdrec, 1, -1))
      {
        tWagoSnmpMsg stMessage;
        if(0 < mq_receive(trap_queue, (char *) &stMessage,
                          sizeof(tWagoSnmpMsg),NULL))
        {
          switch(stMessage.type)
          {
            case MSG_TYPE_TRAP_EASY:
              send_easy_trap(6,stMessage.specific_type);
              break;
            case MSG_TYPE_TRAP:
              _TrapSend(&stMessage);
              break;
            case MSG_TYPE_REGISTER_OID:
              _RegisterOID(&stMessage);
              break;
            case MSG_TYPE_RESET:
              _Reset();
              break;
            case MSG_TYPE_NONE:
            default:
              break;
          }
        }
      }
    }

  return 0;
}


static mqd_t _OpenServerQueue(const char * name, long int msgsz, long int maxmsg)
{
  mqd_t ret = -1;
  struct mq_attr mqAttr;

  mqAttr.mq_flags = 0;
  mqAttr.mq_maxmsg = maxmsg;//create a good size for the message buffer
  mqAttr.mq_msgsize = msgsz;
  ret = mq_open(name, OPEN_SERVER_MODE, CREAT_MODE, &mqAttr );
  if(ret < 0)
  {
    mq_unlink(name);
    ret = mq_open(name, OPEN_SERVER_MODE, CREAT_MODE, &mqAttr );
  }

  return ret;
}

static void _InitExistingShm(void)
{
  AGENT_MutexLock();
  AGENT_CreateShm();
  if(oidShmFd >= 0)
  {
    tOidObject * pAct = AGENT_GetNextOidObject(NULL);
    while(pAct != NULL)
    {
      _RegisterOIDHandler(pAct->anOID, pAct->anOID_length);
      pAct = AGENT_GetNextOidObject(pAct);
    }
  }
  AGENT_MutexUnlock();
}

INTERNAL_SYM void AGENT_InitServerCommunication(unsigned int clientreg, void *clientarg)
{
  (void) clientreg;
  (void) clientarg;
  snmp_log  (LOG_INFO,"Init WAGO snmp plugin\n");

  _InitExistingShm();

  trap_queue = _OpenServerQueue(TRAP_AGENT_MQ,sizeof(tWagoSnmpMsg), 1);

  if((pthread_create(&stThreadID, NULL, _ServerMain, NULL)) == -1)
    DEBUGMSGTL(("plcsnmp_trap_agent", "error while starting thread\n"));
  else
    DEBUGMSGTL(("plcsnmp_trap_agent", "starting thread ok\n"));
}

INTERNAL_SYM int AGENT_CreateMutex()
{
  int ret = 0;
  if(oidMutex == NULL)
  {
    oidMutex = sem_open(WAGO_SNMP_OID_MUTEX,O_RDWR|O_CREAT|O_EXCL, 0666,1);
    if(oidMutex == SEM_FAILED)
    {
      if(errno == EEXIST)
      {
        oidMutex = sem_open(WAGO_SNMP_OID_MUTEX,O_RDWR);
      }
      if(oidMutex == SEM_FAILED)
      {
        ret = -1;
        oidMutex = NULL;
      }
    }
  }
  return ret;
}

INTERNAL_SYM void AGENT_MutexLock()
{
  if(AGENT_CreateMutex() >= 0)
  {
    sem_wait(oidMutex);
  }
}
INTERNAL_SYM void AGENT_MutexUnlock()
{
  if(AGENT_CreateMutex() >= 0)
  {
    sem_post(oidMutex);
  }
}

INTERNAL_SYM void AGENT_CloseMutex()
{
  if(oidMutex != NULL)
  {
    sem_close(oidMutex);
    oidMutex=NULL;
  }
}

INTERNAL_SYM void AGENT_DestroyMutex()
{
  if(AGENT_CreateMutex() >= 0)
  {
    AGENT_CloseMutex();
    sem_unlink(WAGO_SNMP_OID_MUTEX);
  }
}

INTERNAL_SYM void AGENT_RemapShm()
{
  int newSize;
  //Compile Errors
 // pOidShm = mremap(pOidShm,localShmSize,pOidShm->oidShmSize, MREMAP_MAYMOVE);
  newSize = pOidShm->oidShmSize;
  munmap(pOidShm,localShmSize);
  pOidShm = mmap(0,newSize,PROT_READ|PROT_WRITE,MAP_SHARED,oidShmFd,0);
  localShmSize = newSize;
}

INTERNAL_SYM void AGENT_CreateShm()
{
  if(oidShmFd < 0)
  {
    oidShmFd = shm_open(WAGO_SNMP_OID_SHM,O_RDWR|O_CREAT|O_EXCL, 0666);
    if(oidShmFd < 0)
    {
      if (errno == EEXIST)
      {
        oidShmFd = shm_open(WAGO_SNMP_OID_SHM,O_RDWR,0666);
        pOidShm = mmap(0,sizeof(tOidShm),PROT_READ|PROT_WRITE,MAP_SHARED,oidShmFd,0);
        AGENT_RemapShm();
      }
    }
    else
    {
      ftruncate(oidShmFd,sizeof(tOidShm));
      pOidShm = mmap(0,sizeof(tOidShm),PROT_READ|PROT_WRITE,MAP_SHARED,oidShmFd,0);
      pOidShm->oidShmSize = sizeof(tOidShm);
      localShmSize = pOidShm->oidShmSize;
      pOidShm->oidCount = 0;
    }
  }
}

INTERNAL_SYM void AGENT_CloseShm(void)
{
  if(oidShmFd >= 0)
  {
    munmap(pOidShm,pOidShm->oidShmSize);
    pOidShm = NULL;
    close(oidShmFd);
    oidShmFd = -1;
    localShmSize = sizeof(tOidShm);
  }
}

INTERNAL_SYM void AGENT_DestroyShm(void)
{
    AGENT_CloseShm();
    shm_unlink(WAGO_SNMP_OID_SHM);
}

/* adding size bytes to SHM*/
INTERNAL_SYM int AGENT_ExtendShm(int size)
{
  int ret = -1;
  if(oidShmFd >= 0)
  {
    int newSize = size + pOidShm->oidShmSize;
    ftruncate(oidShmFd,newSize);
    //Compile Errors
    //pOidShm = mremap(pOidShm, pOidShm->oidShmSize,newSize, MREMAP_MAYMOVE);
    munmap(pOidShm,pOidShm->oidShmSize);
    pOidShm = mmap(0,newSize,PROT_READ|PROT_WRITE,MAP_SHARED,oidShmFd,0);
    pOidShm->oidShmSize = newSize;
    ret = 0;
  }

  return ret;
}

INTERNAL_SYM tOidObject * AGENT_GetNextOidObject(tOidObject * pAct)
{
  if(pAct == NULL )
  {
    if(pOidShm->oidCount > 0)
    {
      return &pOidShm->oidStart[0];
    }
    else
    {
      return NULL;
    }
  }
  //if this is the last index
  if(pOidShm->oidCount <= (pAct->index+1))
  {
    return NULL;
  }

   //CALC_OBJ_SIZE(step,pAct->len,pAct->len <= OID_BUFFER_LEN);
  pAct = (tOidObject*) ( ((unsigned int) pAct) + ((unsigned int)  pAct->objLen));
  return pAct;
}

INTERNAL_SYM tOidObject * AGENT_GetOidObject(oid * anOID,size_t anOID_len)
{
  tOidObject * ret = NULL;
  AGENT_CreateShm();
  if(oidShmFd >= 0)
  {
    tOidObject * pAct = AGENT_GetNextOidObject(NULL);
    while(pAct != NULL)
    {
      if(0==snmp_oid_compare  (anOID,anOID_len,pAct->anOID,pAct->anOID_length))
      {
        ret=pAct;
        break;
      }
      pAct = AGENT_GetNextOidObject(pAct);
    }
  }
  return ret;
}

INTERNAL_SYM tOidObject * AGENT_GetFreeOidObject(int size)
{
  tOidObject * pAct;
  tOidObject * pLast = NULL;
  /*pAct = AGENT_GetNextOidObject(NULL);
  while(pAct != NULL)
  {
    pLast = pAct;
    pAct = AGENT_GetNextOidObject(pAct);
  }*/
  AGENT_ExtendShm(size);
  pAct = AGENT_GetNextOidObject(NULL);
  while(pAct != NULL)
  {
    pLast = pAct;
    pAct = AGENT_GetNextOidObject(pAct);
  }
  pOidShm->oidCount++;
  pAct = AGENT_GetNextOidObject(pLast);
  if(pAct != NULL)
  {
    pAct->objLen = size;
    pAct->index = pOidShm->oidCount-1;
  }
  return pAct;
}

INTERNAL_SYM void AGENT_SetOidObjectValue(tOidObject * object,netsnmp_variable_list * stData)
{
  size_t bufferLen = object->objLen - sizeof(tOidObject)+OID_BUFFER_LEN;
  object->type = stData->type;
  if(bufferLen >= stData->val_len)
  {
    memcpy(object->buf, stData->val.string,stData->val_len);
    object->len = stData->val_len;
    object->buf[stData->val_len]=0;
  }
  else
  {
    memcpy(object->buf, stData->val.string,bufferLen);
    object->len = bufferLen;
    object->buf[bufferLen]=0;
  }
}

INTERNAL_SYM tWagoSnmpReturnCode AGENT_CreateNewOidObject(oid * anOID, size_t anOID_len,netsnmp_variable_list * stData,uint8_t readOnly)
{
  tWagoSnmpReturnCode result = WAGOSNMP_RETURN_ERROR_SHM;
  AGENT_CreateShm();

  if(oidShmFd >= 0)
  {
    tOidObject * pObj;
    int objectSize;

    CALC_OBJ_SIZE(objectSize,stData->val_len,stData->val.string == stData->buf);
    AGENT_MutexLock();
    pObj = AGENT_GetFreeOidObject(objectSize);

    if(pObj != NULL)
    {
      pObj->readOnly = readOnly;
      memcpy(pObj->anOID,anOID,anOID_len*sizeof(oid));
      pObj->anOID_length = anOID_len;
      AGENT_SetOidObjectValue(pObj,stData);
      result = WAGOSNMP_RETURN_OK;
    }
    AGENT_MutexUnlock();
  }

  return result;
}


//---- End of source file ------------------------------------------------------
