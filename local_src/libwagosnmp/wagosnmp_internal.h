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
///  \file     wagosnmp_internal.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGOSNMP_INTERNAL_H_
#define WAGOSNMP_INTERNAL_H_

#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <poll.h>
#include <errno.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#define PUBLIC_SYM __attribute__((visibility ("default")))
#define INTERNAL_SYM __attribute__((visibility ("internal")))

INTERNAL_SYM extern pthread_once_t snmp_is_initialized;
INTERNAL_SYM extern pthread_once_t snmp_agent_is_initialized;
#define INIT_SNMP_ONCE (void)pthread_once(&snmp_is_initialized, INTERNAL_InitSnmp);
#define INIT_SNMP_AGENT_ONCE INIT_SNMP_ONCE;(void)pthread_once(&snmp_agent_is_initialized, INTERNAL_InitSnmpAgent);

#define SNMP_TLV(x) ((netsnmp_variable_list *)x)
#if 0
#define WAGOSNMP_INTEGER                   ASN_INTEGER
#define WAGOSNMP_OCTET_STR                 ASN_OCTET_STR
#define WAGOSNMP_BIT_STR                   ASN_BIT_STR
#define WAGOSNMP_OBJECT_ID                 ASN_OBJECT_ID
#define WAGOSNMP_IP_ADDRESS                ASN_IPADDRESS
#define WAGOSNMP_COUNTER                   ASN_COUNTER
#define WAGOSNMP_GAUGE                     ASN_GAUGE
#define WAGOSNMP_UNSIGNED                  ASN_UNSIGNED
#define WAGOSNMP_TIMETICKS                 ASN_TIMETICKS
#define WAGOSNMP_COUNTER64                 ASN_COUNTER
#define WAGOSNMP_NULL                      ASN_NULL
#endif


#define WAGOSNMP_INIT_NAME                 "wagosnmp"
#define WAGOSNMP_AGENT_NAME                "wagosnmpagent"
#define WAGO_SNMP_AGENT_MQ                 "/WAGO_SNMP_AGENT"
#define WAGO_SNMP_OID_MUTEX                "/WAGO_SNMP_OID_MUTEX"
#define WAGO_SNMP_OID_SHM                  "/WAGO_SNMP_OID_SHM"
#define TRAP_AGENT_MQ                       WAGO_SNMP_AGENT_MQ

#define OPEN_CLIENT_MODE       (O_WRONLY|O_NONBLOCK)
#define CREAT_MODE      (S_IWUSR|S_IRUSR)
#define OPEN_SERVER_MODE       (O_RDONLY|O_CREAT|O_EXCL|O_NONBLOCK)

#define OID_BUFFER_LEN  256

/*if(pAct->len <= OID_BUFFER_LEN)
  {
    step = sizeof(tOidObject);
  }
  else
  {
    step = CALC_OBJ_SIZE(pAct->len);
  }*/
//#define CALC_OBJ_SIZE(x) (sizeof(tOidObject) + (x -OID_BUFFER_LEN))
#define CALC_OBJ_SIZE(x,y,z) { if(z)\
                           { x = sizeof(tOidObject);}\
                           else{ x = (sizeof(tOidObject) + (y -OID_BUFFER_LEN));}}

#define SNMP_TLV_INITALIZED(x) (SNMP_TLV(x)->type != 0)
#define SNMP_TLV_IS_NULL(x) (SNMP_TLV(x)->type == ASN_NULL)

typedef enum {
  MSG_TYPE_TRAP_EASY,
  MSG_TYPE_TRAP,
  MSG_TYPE_REGISTER_OID,
  MSG_TYPE_RESET,
  MSG_TYPE_NONE
}tMsgTrapType;

typedef struct {
    oid      sOID[MAX_OID_LEN];
    size_t   sOID_length;
    uint16_t type;//z.B. ASN_UNSIGNED
    uint16_t len;
    uint8_t  buf[256];
}tTrapVariableType;

typedef struct {
    tMsgTrapType      type;
    oid               enterprise[MAX_OID_LEN];
    size_t            enterprise_length;
    long              trap_type;
    long              specific_type;
    unsigned char     agent_addr[4];
    tTrapVariableType variable;
}tWagoSnmpMsg;

typedef struct {
    uint32_t index;
    size_t   objLen;
    uint8_t  readOnly;
    oid      anOID[MAX_OID_LEN];
    size_t   anOID_length;
    uint16_t type;
    uint16_t len;
    uint8_t  buf[OID_BUFFER_LEN];
}__attribute__ ((packed)) tOidObject;

typedef struct {
    int         oidShmSize;
    int         oidCount;
    tOidObject  oidStart[];
}__attribute__ ((packed)) tOidShm;



int INTERNAL_SnmpInput(int operation,
           netsnmp_session * session,
           int reqid, netsnmp_pdu *pdu, void *magic);
INTERNAL_SYM int INTERNAL_SetAuthPriv(tWagoSnmpTranceiver * trcv,
                        netsnmp_session     * session);
INTERNAL_SYM void INTERNAL_InitSnmp(void);
INTERNAL_SYM void INTERNAL_ResetSnmpAgent(void);
INTERNAL_SYM void INTERNAL_InitSnmpAgent(void);
INTERNAL_SYM void INTERNAL_ReTwist(netsnmp_variable_list * stData);
INTERNAL_SYM int  INTERNAL_SetVarTypedValue(netsnmp_variable_list * stData, u_char eType, u_char * input,size_t size);
INTERNAL_SYM void INTERNAL_DeTwist(netsnmp_variable_list * stData);
INTERNAL_SYM int  INTERNAL_SnprintValue(char *buf, size_t buf_len,
              const oid * objid, size_t objidlen,
              const netsnmp_variable_list * variable);
INTERNAL_SYM char *INTERNAL_StripQuotes(char * p);
INTERNAL_SYM int INTERNAL_Tranceive(tWagoSnmpTranceiver * trcv, netsnmp_pdu *pdu,netsnmp_session *ss);
INTERNAL_SYM int INTERNAL_GetSnmpPdu(tWagoSnmpTranceiver * trcv, netsnmp_pdu **pdu);
INTERNAL_SYM tWagoSnmpReturnCode INTERNAL_GetSnmpSession(tWagoSnmpTranceiver * trcv, netsnmp_session **ss);
INTERNAL_SYM int INTERNAL_ConvertTlvToTrapVar(tTrapVariableType * var, netsnmp_variable_list * stData);
INTERNAL_SYM int INTERNAL_SendTrapMsg(tWagoSnmpMsg * msg);
INTERNAL_SYM int INTERNAL_SendReleaseOIDs(void);
INTERNAL_SYM int INTERNAL_InformForNewOid(tOidObject * object);
INTERNAL_SYM netsnmp_session* INTERNAL_GenerateSession_v1_v2c(char sHost[128],
                                                              char sCommunity[64],
                                                              long version);
INTERNAL_SYM int INTERNAL_AddVarAndSend(char                    sOID[128],
                                        netsnmp_variable_list * stData,
                                        netsnmp_session       * ss,
                                        netsnmp_pdu           * pdu);
INTERNAL_SYM netsnmp_pdu* INTERNAL_GetTrap2PDU_v2_v3(char                    sEnterprise[128],
                                                     tWagoSnmpReturnCode *   result);

/* AGENT */
INTERNAL_SYM void AGENT_InitServerCommunication(unsigned int clientreg, void *clientarg);
INTERNAL_SYM void AGENT_RemapShm(void);
INTERNAL_SYM int  AGENT_CreateMutex(void);
INTERNAL_SYM void AGENT_MutexLock(void);
INTERNAL_SYM void AGENT_MutexUnlock(void);
INTERNAL_SYM void AGENT_CloseMutex(void);
INTERNAL_SYM void AGENT_DestroyMutex(void);
INTERNAL_SYM void AGENT_CreateShm(void);
INTERNAL_SYM void AGENT_CloseShm(void);
INTERNAL_SYM void AGENT_DestroyShm(void);
INTERNAL_SYM int  AGENT_ExtendShm(int size);

INTERNAL_SYM tOidObject * AGENT_GetNextOidObject(tOidObject * pAct);
INTERNAL_SYM tOidObject * AGENT_GetOidObject(oid * anOID,size_t anOID_len);
INTERNAL_SYM tOidObject * AGENT_GetFreeOidObject(int size);
INTERNAL_SYM void AGENT_SetOidObjectValue(tOidObject* pObj,netsnmp_variable_list*stData);
INTERNAL_SYM tWagoSnmpReturnCode AGENT_CreateNewOidObject(oid * anOID,
                                                          size_t anOID_len,
                                                          netsnmp_variable_list * stData,uint8_t readOnly);

/* libnetsnmp */

INTERNAL_SYM void SNMP_MutexLock(void);
INTERNAL_SYM void SNMP_MutexUnlock(void);



#endif /* WAGOSNMP_INTERNAL_H_ */
//---- End of source file ------------------------------------------------------
