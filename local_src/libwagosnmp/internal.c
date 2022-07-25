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
///  \file     internal.c
///
///  \version  $Rev: 66937 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include "error.h"
#include "wagosnmp_API.h"
#include "wagosnmp_internal.h"

INTERNAL_SYM pthread_once_t snmp_is_initialized       = PTHREAD_ONCE_INIT;
INTERNAL_SYM pthread_once_t snmp_agent_is_initialized = PTHREAD_ONCE_INIT;
//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
static const oid objid_sysuptime[] = {1, 3, 6, 1, 2, 1, 1, 3, 0};
static const oid objid_snmptrap[]  = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

#define USE_OWN_COPY_FUNCTION 0

//-- Function: _SprintReallocValue -----------------------------------------
///
///  Copy/Paste from snmp-> snmplib/mib.c.sprint_realloc_by_type
///  This functions is changed, so that the output will not consider the MIB definitions
///
//-----------------------------------------------------------------------------
static int _SprintReallocValue(u_char **buf, size_t *buf_len, size_t *out_len, int allow_realloc, const oid *objid,
                               size_t objidlen, const netsnmp_variable_list *variable) {
  if (variable->type == SNMP_NOSUCHOBJECT) {
    return snmp_strcat(buf, buf_len, out_len, allow_realloc,
                       (const u_char *)"No Such Object available on this agent at this OID");
  } else if (variable->type == SNMP_NOSUCHINSTANCE) {
    return snmp_strcat(buf, buf_len, out_len, allow_realloc,
                       (const u_char *)"No Such Instance currently exists at this OID");
  } else if (variable->type == SNMP_ENDOFMIBVIEW) {
    return snmp_strcat(buf, buf_len, out_len, allow_realloc,
                       (const u_char *)"No more variables left in this MIB View (It is past the end of the MIB tree)");
  } else {
    return sprint_realloc_by_type(buf, buf_len, out_len, allow_realloc, variable, NULL, NULL, NULL);
  }
}

//-- Function: _CopySnmpVarData -------------------------------------------------
///
///  Copy nvl Data from the SNMP-created instancies to the PLC-Created instancys!
///
///  \param dest: Pointer to the MSG-Instancy
///  \param src:  Pointer to the SNMP-Instancy
///
//-----------------------------------------------------------------------------
static void _CopySnmpVarData(tWagoSnmpTranceiver *trcv, netsnmp_variable_list *src) {
#if USE_OWN_COPY_FUNCTION
  u_char *strOld = NULL;
  netsnmp_variable_list *pNext;
  // perform precheck
  if (src == NULL) {
    return;
  }

  // Mint the order of this contruct!!!
  // if we create a new string it may be freed in the IEC-thread!!!
  // it may end up in a race-condition with duble-free an segvault!!!
  pNext              = src->next_variable;
  src->next_variable = NULL;

  strOld          = src->val.string;
  src->val.string = NULL;

  memcpy(trcv->typData, src, sizeof(netsnmp_variable_list));

  src->next_variable = pNext;
  src->val.string    = strOld;
  if (src->buf != src->val.string) {
    u_char *pNew = malloc((src->val_len));
    memcpy(pNew, src->val.string, src->val_len);
    trcv->sDataString                      = (char *)pNew;
    SNMP_TLV(trcv->typData)->next_variable = SNMP_TLV(trcv->typData);
  } else {
    trcv->sDataString                      = NULL;
    SNMP_TLV(trcv->typData)->next_variable = NULL;
  }
#else
  if (src == NULL) {
    return;
  }
  if (SNMP_TLV(trcv->typData)->val.string && SNMP_TLV(trcv->typData)->val.string != SNMP_TLV(trcv->typData)->buf) {
    free(SNMP_TLV(trcv->typData)->val.string);
  }
  (void)snmp_clone_var(src, SNMP_TLV(trcv->typData));
  INTERNAL_DeTwist(SNMP_TLV(trcv->typData));

#endif
}

static mqd_t _OpenClientQueue(const char *name, long int msgsz, long int maxmsg) {
  mqd_t ret = -1;
  struct mq_attr mqAttr;
  errno = 0;

  mqAttr.mq_flags   = 0;
  mqAttr.mq_maxmsg  = maxmsg;  // create a good size for the message buffer
  mqAttr.mq_msgsize = msgsz;
  ret               = mq_open(name, OPEN_CLIENT_MODE, CREAT_MODE, &mqAttr);
  // SNMP_TRAP_COLDSTART;
  if (ret < 0) {
    perror("mq_open");
  }

  return ret;
}

//-- Function: _SetAuthPriv ----------------------------------------------------
///
///  Set the authority configurations
///
///  \param msg      actual worked out message
///  \param session  pointer to the actaul snmp-session var
///
//------------------------------------------------------------------------------
INTERNAL_SYM int INTERNAL_SetAuthPriv(tWagoSnmpTranceiver *trcv, netsnmp_session *session) {
  int ret = WAGOSNMP_RETURN_OK;
  // check if we need to do the auth and priv settings
  if (session->securityLevel == SNMP_SEC_LEVEL_NOAUTH) {
    return ret;
  }

  // set the Authetication variables
  switch (trcv->typAuthProt) {
    case WAGOSNMP_AUTH_P_NONE:
      session->securityAuthProto    = usmNoAuthProtocol;
      session->securityAuthProtoLen = sizeof(usmNoAuthProtocol) / sizeof(oid);
      break;
    case WAGOSNMP_AUTH_P_MD5:
      /* set the authentication method to MD5 */
      session->securityAuthProto    = usmHMACMD5AuthProtocol;
      session->securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol) / sizeof(oid);
      break;
    case WAGOSNMP_AUTH_P_SHA:
      /* set the authentication method to MD5 */
      session->securityAuthProto    = usmHMACSHA1AuthProtocol;
      session->securityAuthProtoLen = sizeof(usmHMACSHA1AuthProtocol) / sizeof(oid);
      break;
    default:
      // waht to do in case of error??
      break;
  }
  session->securityAuthKeyLen = USM_AUTH_KU_LEN;
  /* set the authentication key to a hashed version of our
     passphrase (which must be at least 8 characters long) */
  if (generate_Ku(session->securityAuthProto, session->securityAuthProtoLen, (u_char *)trcv->sAuthPass,
                  strlen(trcv->sAuthPass), session->securityAuthKey, &session->securityAuthKeyLen) != SNMPERR_SUCCESS) {
    snmp_log(LOG_ERR, "Error generating Ku from authentication pass phrase. \n");
    ret = WAGOSNMP_RETURN_AUTH_ERR;
  }
  // set the Privacy variables
  if (session->securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
    switch (trcv->typPrivProt) {
      case WAGOSNMP_PRIV_P_NONE:
        session->securityPrivProto    = usmNoPrivProtocol;
        session->securityPrivProtoLen = sizeof(usmNoPrivProtocol) / sizeof(oid);
        break;
      case WAGOSNMP_PRIV_P_DES:
        /* set the authentication method to MD5 */
        session->securityPrivProto    = usmDESPrivProtocol;
        session->securityPrivProtoLen = sizeof(usmDESPrivProtocol) / sizeof(oid);
        break;
      case WAGOSNMP_PRIV_P_AES:
        /* set the authentication method to MD5 */
        session->securityPrivProto    = usmAESPrivProtocol;
        session->securityPrivProtoLen = sizeof(usmAESPrivProtocol) / sizeof(oid);
        break;
      default:
        // waht to do in case of error??
        break;
    }
    session->securityPrivKeyLen = USM_PRIV_KU_LEN;
    /* set the privacy key to a hashed version of our
       passphrase (which must be at least 8 characters long) */
    if (generate_Ku(session->securityAuthProto, session->securityAuthProtoLen, (u_char *)trcv->sPrivPass,
                    strlen(trcv->sPrivPass), session->securityPrivKey,
                    &session->securityPrivKeyLen) != SNMPERR_SUCCESS) {
      snmp_log(LOG_ERR, "Error generating Ku from privacy pass phrase. \n");
      ret = WAGOSNMP_RETURN_PRIV_ERR;
    }
  }
  return ret;
}

INTERNAL_SYM int INTERNAL_SnmpInput(int operation, netsnmp_session *session, int reqid, netsnmp_pdu *pdu, void *magic) {
  return 1;
}

INTERNAL_SYM void INTERNAL_InitSnmp(void) {
  init_snmp("snmpapp");
}

INTERNAL_SYM void INTERNAL_ResetSnmpAgent() {
  // INIT_SNMP_ONCE;
  INTERNAL_SendReleaseOIDs();
  AGENT_DestroyShm();
  AGENT_DestroyMutex();
}
INTERNAL_SYM void INTERNAL_InitSnmpAgent() {
  // INIT_SNMP_ONCE;
  INTERNAL_ResetSnmpAgent();
  AGENT_CreateMutex();
  AGENT_CreateShm();
}

//-- Function: _ReTwist ---------------------
///
///  The SNMP-API uses two ways to buffer data.
///  1. a 40 Byte array from inside the netsnmp_variable_list structure
///  2. a dynamically allocated array.
///  the library copys the nvl instancy. after a copy the val pointer my point
///  to the 40 byte buffe of another (no longer existing) instancy. this can
///  in memory exceptions.
///  This Function hlps to reoder the internla used instancys of nvl which was
///  correted before.
///
///  \param stData:  Instancy of nvl to be reordered;
///
//-------------------------------------------------------------------------
INTERNAL_SYM void INTERNAL_ReTwist(netsnmp_variable_list *stData) {
  if (stData->val.string == NULL) {
    stData->val.string = stData->buf;
  }
}

INTERNAL_SYM int INTERNAL_SetVarTypedValue(netsnmp_variable_list *stData, u_char eType, u_char *input, size_t size) {
#if USE_OWN_COPY_FUNCTION
  if (stData->next_variable != NULL) {
    netsnmp_variable_list *pNext = stData->next_variable;
    if (pNext->buf != pNext->val.string) {
      pNext->val.string = NULL;
    }
    stData->next_variable = NULL;
  }
#endif
  return snmp_set_var_typed_value(stData, (u_char)eType, input, size);
}

//-- Function: _DeTwist ---------------------
///
///  The SNMP-API uses two ways to buffer data.
///  1. a 40 Byte array from inside the netsnmp_variable_list structure
///  2. a dynamically allocated array.
///  the library copys the nvl instancy. after a copy the val pointer my point
///  to the 40 byte buffe of another (no longer existing) instancy. this can
///  in memory exceptions.
///  this Function sets the pointer which target to the value to NULL in case of
///  using the internal Buffer
///
///  \param stData:  Instancy of nvl to be reordered;
///
//-------------------------------------------------------------------------
INTERNAL_SYM void INTERNAL_DeTwist(netsnmp_variable_list *stData) {
  if (stData->val.string == stData->buf) {
    stData->val.string = NULL;
  }
}

//-- Function: _SnprintValue -----------------------------------------
///
///  Copy/Paste from snmp-> snmplib/mib.c snprint_value
///  Only a Wrapper for _SprintReallocValue to be equal to the snprin_value way
///
//-----------------------------------------------------------------------------
INTERNAL_SYM int INTERNAL_SnprintValue(char *buf, size_t buf_len, const oid *objid, size_t objidlen,
                                       const netsnmp_variable_list *variable) {
  size_t out_len = 0;

  if (_SprintReallocValue((u_char **)&buf, &buf_len, &out_len, 0, objid, objidlen, variable)) {
    return (int)out_len;
  } else {
    return -1;
  }
}

INTERNAL_SYM char *INTERNAL_StripQuotes(char *p) {
  if (*p == '\"' && p[strlen(p) - 1] == '\"') {
    p++;
    p[strlen(p) - 1] = 0;
  }
  return p;
}

//-- Function: _GetSnmpSession -------------------------------------------------
///
///  Initiate the new SNMP-Session for all Protocol Versions
///
///  \param msg: Pointer to the actual used Message
///  \param ss:  Pointer to the newly created session-poiner
///
//------------------------------------------------------------------------------
INTERNAL_SYM tWagoSnmpReturnCode INTERNAL_GetSnmpSession(tWagoSnmpTranceiver *trcv, netsnmp_session **ss) {
  netsnmp_session tmpss;
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;

  INIT_SNMP_ONCE;
  snmp_sess_init(&tmpss);
  tmpss.version = trcv->version;
  tmpss.retries = trcv->retries;
  tmpss.timeout = trcv->timeout_us;
  if (trcv->sHost[0] != 0) {
    // tmpss.peername = strdup(msg->trcvData.sHost);//warum hier strdup und oben nicht?
    tmpss.peername = trcv->sHost;  // ich sehe nicht warum das nihct gehen sollte
  }

  if ((trcv->version == SNMP_VERSION_1) || (trcv->version == SNMP_VERSION_2c)) {
    /* set the SNMPv1 community name used for authentication */
    tmpss.community     = (u_char *)trcv->sCommunity;
    tmpss.community_len = strlen((char *)tmpss.community);
  } else {
    /* set the SNMPv3 user name */
    // tmpss.securityName = strdup(msg->trcvData.sUsername);
    tmpss.securityName    = trcv->sUsername;  // s.o.
    tmpss.securityNameLen = strlen(tmpss.securityName);

    /* set the security level to authenticated, but not encrypted */
    tmpss.securityLevel = trcv->typSecLevel;
    ret                 = INTERNAL_SetAuthPriv(trcv, &tmpss);
  }
  if (trcv->trcvType == WAGOSNMP_TRCV_INFORM) {
    tmpss.callback       = INTERNAL_SnmpInput;
    tmpss.callback_magic = NULL;
    setup_engineID(NULL, NULL);
    if (tmpss.contextEngineIDLen == 0 || tmpss.contextEngineID == NULL) {
      tmpss.contextEngineID = snmpv3_generate_engineID(&tmpss.contextEngineIDLen);
    }
#if 1
    if (tmpss.version == SNMP_VERSION_3 && trcv->sEngineId != NULL && trcv->sEngineId[0] != 0) {
      size_t ebuf_len = 32, eout_len = 0;
      u_char *ebuf = (u_char *)malloc(ebuf_len);

      if (!snmp_hex_to_binary(&ebuf, &ebuf_len, &eout_len, 1, trcv->sEngineId)) {
        fprintf(stderr, "Bad engine ID value.\n");
        free(ebuf);
        ret = WAGOSNMP_RETURN_ERROR_BAD_ENGINE_ID;
      }
      if ((eout_len < 5) || (eout_len > 32)) {
        fprintf(stderr, "Invalid engine ID value\n");
        free(ebuf);
        ret = WAGOSNMP_RETURN_ERROR_INVALID_ENGINE_ID;
      }
      tmpss.securityEngineID    = ebuf;
      tmpss.securityEngineIDLen = eout_len;
#if 0
      if (   (tmpss.securityEngineIDLen == 0)
          || (tmpss.securityEngineID == NULL))
      {
          tmpss.securityEngineID = snmpv3_generate_engineID(&tmpss.securityEngineIDLen);
      }

                /*
                 * set boots and time, which will cause problems if this
                 * machine ever reboots and a remote trap receiver has cached our
                 * boots and time...  I'll cause a not-in-time-window report to
                 * be sent back to this machine.
                 */
                if (tmpss.engineBoots == 0)
                  tmpss.engineBoots = 1;
                if (tmpss.engineTime == 0)    /* not really correct, */
                  tmpss.engineTime = get_uptime();  /* but it'll work. Sort of. */
#endif
    }
#endif
    if (ret == WAGOSNMP_RETURN_OK) {
      netsnmp_transport *transport = netsnmp_transport_open_client("snmptrap", tmpss.peername);
      *ss                          = snmp_add(&tmpss, transport, NULL, NULL);
    }
  } else {
    if (ret == WAGOSNMP_RETURN_OK) {
      *ss = snmp_open(&tmpss);
    }
  }
  if (!*ss) {
    snmp_sess_perror("snmp session error", &tmpss);  // this function is badly described!!!
    if (ret == WAGOSNMP_RETURN_OK) ret = WAGOSNMP_RETURN_INIT_SESSION_ERROR;
  }
  return ret;
}

//-- Function: _GetSnmpSession -------------------------------------------------
///
///  Create the PDU for the transcieve!
///
///  \param msg: Pointer to the actual used Message
///  \param pdu:  Pointer to the newly created pdu-poiner
///
//------------------------------------------------------------------------------
INTERNAL_SYM int INTERNAL_GetSnmpPdu(tWagoSnmpTranceiver *trcv, netsnmp_pdu **pdu) {
  int type         = 0;
  size_t anOID_len = MAX_OID_LEN;
  oid anOID[MAX_OID_LEN];

  if ((trcv->trcvType == WAGOSNMP_TRCV_GET)) {
    type = SNMP_MSG_GET;
  } else if (trcv->trcvType == WAGOSNMP_TRCV_SET) {
    type = SNMP_MSG_SET;
  } else if (trcv->trcvType == WAGOSNMP_TRCV_INFORM) {
    type = SNMP_MSG_INFORM;
  } else {
    return WAGOSNMP_RETURN_BAD_MSG_TYPE;
  }

  if (trcv->sOID != NULL && trcv->sOID[0] != 0) {
    anOID_len = MAX_OID_LEN;
    SNMP_MutexLock();
    if (!snmp_parse_oid(trcv->sOID, anOID, &anOID_len)) {
      SNMP_MutexUnlock();
      snmp_perror(trcv->sOID);
      return WAGOSNMP_RETURN_PARSE_OID_ERROR;
    }
    SNMP_MutexUnlock();
  } else {
    return WAGOSNMP_RETURN_ERROR_PARAMETER;
  }

  *pdu = snmp_pdu_create(type);

  if (type == SNMP_MSG_GET) {
    snmp_add_null_var(*pdu, anOID, anOID_len);
  } else if (type == SNMP_MSG_SET || type == SNMP_MSG_INFORM) {
    u_char *buf;

    if (type == SNMP_MSG_INFORM) {
      u_long uptime;
      static const oid objid_sysuptime[] = {1, 3, 6, 1, 2, 1, 1, 3, 0};
      static const oid objid_snmptrap[]  = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

      // If additional OID is given but tlv is uninitialized return error!
      // uninitializes will be recocnised ny type == 0
      // type == 0 is ASN UNIVERSAL but actually this will not be used
      if (((trcv->sInformOID == NULL || trcv->sInformOID[0] == 0) &&
           (SNMP_TLV_INITALIZED(trcv->typData) && !SNMP_TLV_IS_NULL(trcv->typData))) ||
          ((trcv->sInformOID != NULL && trcv->sInformOID[0] != 0) &&
           (!SNMP_TLV_INITALIZED(trcv->typData) || SNMP_TLV_IS_NULL(trcv->typData)))) {
        snmp_free_pdu(*pdu);
        return WAGOSNMP_RETURN_ERROR_PARAMETER;
      }

      uptime = get_uptime();
      if (NULL == snmp_pdu_add_variable(*pdu, objid_sysuptime, sizeof(objid_sysuptime) / sizeof(oid), ASN_TIMETICKS,
                                        &uptime, sizeof(uptime))) {
        snmp_free_pdu(*pdu);
        return WAGOSNMP_RETURN_ERR_MALLOC;
      }
      if (NULL == snmp_pdu_add_variable(*pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid), ASN_OBJECT_ID,
                                        anOID, anOID_len * sizeof(oid))) {
        snmp_free_pdu(*pdu);
        return WAGOSNMP_RETURN_ERR_MALLOC;
      }
      // get InformOID into the anOID buffer for the additional OID
      anOID_len = MAX_OID_LEN;
      SNMP_MutexLock();
      if ((trcv->sInformOID != NULL && trcv->sInformOID[0] != 0) &&
          (!snmp_parse_oid(trcv->sInformOID, anOID, &anOID_len))) {
        SNMP_MutexUnlock();
        snmp_perror(trcv->sInformOID);
        snmp_free_pdu(*pdu);
        return WAGOSNMP_RETURN_PARSE_OID_ERROR;
      }
      SNMP_MutexUnlock();
    }
    if (!SNMP_TLV_IS_NULL(trcv->typData) && SNMP_TLV_INITALIZED(trcv->typData)) {
      if (SNMP_TLV(trcv->typData)->val.string == NULL) {
        buf = SNMP_TLV(trcv->typData)->buf;
      } else {
        buf = SNMP_TLV(trcv->typData)->val.string;
      }
      snmp_pdu_add_variable(*pdu, anOID, anOID_len, SNMP_TLV(trcv->typData)->type, buf,
                            SNMP_TLV(trcv->typData)->val_len);
    } else if (type == SNMP_MSG_SET) {
      snmp_free_pdu(*pdu);
      return WAGOSNMP_RETURN_BAD_DATATYPE;
    }
  } else {
    snmp_free_pdu(*pdu);
    return WAGOSNMP_RETURN_BAD_MSG_TYPE;
  }

  return WAGOSNMP_RETURN_OK;
}

//-- Function: _GetSnmpSession -------------------------------------------------
///
///  Do the transcive action!
///
///  \param msg: PLC instancys
///  \param pdu: instancy that (will) hold the OID Infomation Structures
///  \param ss:  session Pointer
///
//-----------------------------------------------------------------------------
INTERNAL_SYM int INTERNAL_Tranceive(tWagoSnmpTranceiver *trcv, netsnmp_pdu *pdu, netsnmp_session *ss) {
  int status;
  int ret               = WAGOSNMP_RETURN_OK;
  netsnmp_pdu *response = NULL;

  status = snmp_synch_response(ss, pdu, &response);

  if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
    if (trcv->trcvType == WAGOSNMP_TRCV_GET) {
      _CopySnmpVarData(trcv, response->variables);
    }
  } else {
    if (status == STAT_SUCCESS) {
      fprintf(stderr, "Error in packet 0x%X, 0x%X, %s\nReason: %s\n", trcv->trcvType, (unsigned int)trcv->version,
              trcv->sOID, snmp_errstring(response->errstat));
      ret = WAGOSNMP_RETURN_PACKAGE_ERROR;
    } else if (status == STAT_TIMEOUT) {
      fprintf(stderr, "Timeout: No response from %s.\n", trcv->sHost);
      ret = WAGOSNMP_RETURN_TIMEOUT;
    } else {
      snmp_sess_perror("plcsnmp_manager", ss);
      ret = WAGOSNMP_RETURN_TRANSCEIVE_ERROR;
    }
  }
  if (response) snmp_free_pdu(response);
  SNMP_MutexLock();
  snmp_close(ss);
  // shutdown the app and set reinit flag correctly
  snmp_shutdown("snmpapp");
  snmp_is_initialized = PTHREAD_ONCE_INIT;
  SNMP_MutexUnlock();

  return ret;
}

INTERNAL_SYM int INTERNAL_ConvertTlvToTrapVar(tTrapVariableType *var, netsnmp_variable_list *stData) {
  int ret = WAGOSNMP_RETURN_OK;
  INTERNAL_ReTwist(stData);
  switch (stData->type) {
    case ASN_INTEGER:
    case ASN_TIMETICKS:
    case ASN_COUNTER:
    case ASN_UNSIGNED:
    case ASN_UINTEGER:
    case ASN_COUNTER64:
    case ASN_OPAQUE_FLOAT:
    case ASN_OPAQUE_DOUBLE:
      memcpy(var->buf, stData->val.integer, stData->val_len);
      break;
    case ASN_OBJECT_ID:
    case ASN_PRIV_IMPLIED_OBJECT_ID:
    case ASN_PRIV_INCL_RANGE:
    case ASN_PRIV_EXCL_RANGE:
      memcpy(var->buf, stData->val.objid, stData->val_len);
      break;
    case ASN_IPADDRESS:
    case ASN_PRIV_IMPLIED_OCTET_STR:
    case ASN_OCTET_STR:
    case ASN_BIT_STR:
    case ASN_OPAQUE:
    case ASN_NSAP:
      memcpy(var->buf, stData->val.string, stData->val_len);
      break;
    default:
      ret = WAGOSNMP_RETURN_BAD_DATATYPE;
      break;
  }
  var->type = stData->type;
  var->len  = stData->val_len;
  INTERNAL_DeTwist(stData);
  return ret;
}

INTERNAL_SYM int INTERNAL_SendTrapMsg(tWagoSnmpMsg *msg) {
  int ret = 1;
  struct pollfd fdsnd;
  mqd_t mq;

  mq = _OpenClientQueue(TRAP_AGENT_MQ, sizeof(tWagoSnmpMsg), 1);
  if (mq >= 0) {
    fdsnd.fd      = mq;
    fdsnd.events  = POLLOUT;
    fdsnd.revents = 0;
    if (0 < poll(&fdsnd, 1, -1)) {
      ret = mq_send(mq, (const char *)msg, sizeof(tWagoSnmpMsg), 0);
    }
    mq_close(mq);
  }
  return ret;
}

INTERNAL_SYM int INTERNAL_SendReleaseOIDs(void) {
  int ret = 1;
  struct pollfd fdsnd;
  mqd_t mq;
  tWagoSnmpMsg msg;
  msg.type = MSG_TYPE_RESET;

  mq = _OpenClientQueue(TRAP_AGENT_MQ, sizeof(tWagoSnmpMsg), 1);
  if (mq >= 0) {
    fdsnd.fd      = mq;
    fdsnd.events  = POLLOUT;
    fdsnd.revents = 0;
    if (0 < poll(&fdsnd, 1, -1)) {
      ret = mq_send(mq, (const char *)&msg, sizeof(tWagoSnmpMsg), 0);
    }
    mq_close(mq);
  }
  return ret;
}

INTERNAL_SYM int INTERNAL_InformForNewOid(tOidObject *object) {
  int ret = 1;
  struct pollfd fdsnd;
  mqd_t mq;
  tWagoSnmpMsg msg;
  if (object == NULL) {
    return -1;
  }
  msg.type = MSG_TYPE_REGISTER_OID;
  memcpy(msg.variable.sOID, object->anOID, object->anOID_length * sizeof(oid));
  msg.variable.sOID_length = object->anOID_length;

  mq = _OpenClientQueue(TRAP_AGENT_MQ, sizeof(tWagoSnmpMsg), 1);
  if (mq >= 0) {
    fdsnd.fd      = mq;
    fdsnd.events  = POLLOUT;
    fdsnd.revents = 0;
    if (0 < poll(&fdsnd, 1, -1)) {
      ret = mq_send(mq, (const char *)&msg, sizeof(tWagoSnmpMsg), 0);
    }
    mq_close(mq);
  }
  return ret;
}

INTERNAL_SYM netsnmp_session *INTERNAL_GenerateSession_v1_v2c(char sHost[128], char sCommunity[64], long version) {
  netsnmp_session tmpss;
  netsnmp_session *ss;
  netsnmp_transport *transport;
  INIT_SNMP_ONCE;
  snmp_sess_init(&tmpss);

  tmpss.version        = version;
  tmpss.peername       = sHost;
  tmpss.community      = (u_char *)sCommunity;
  tmpss.community_len  = strlen((char *)tmpss.community);
  tmpss.callback       = INTERNAL_SnmpInput;
  tmpss.callback_magic = NULL;

  setup_engineID(NULL, NULL);
  if (tmpss.contextEngineIDLen == 0 || tmpss.contextEngineID == NULL) {
    tmpss.contextEngineID = snmpv3_generate_engineID(&tmpss.contextEngineIDLen);
  }
  transport = netsnmp_transport_open_client("snmptrap", tmpss.peername);
  if (transport == NULL) {
    return NULL;
  }
  ss = snmp_add(&tmpss, transport, NULL, NULL);

  return ss;
}

INTERNAL_SYM int INTERNAL_AddVarAndSend(char sOID[128], netsnmp_variable_list *stData, netsnmp_session *ss,
                                        netsnmp_pdu *pdu) {
  oid OID[MAX_OID_LEN];
  size_t OID_length;
  tTrapVariableType var;
  int ret = WAGOSNMP_RETURN_OK;
  int status;

  if (ss == NULL) {
    return WAGOSNMP_RETURN_INIT_SESSION_ERROR;
  }

  OID_length = MAX_OID_LEN;
  if (sOID[0] != 0 && !SNMP_TLV_IS_NULL(stData)) {
    SNMP_MutexLock();
    if (!snmp_parse_oid(sOID, OID, &OID_length)) {
      SNMP_MutexLock();
      snmp_perror(sOID);
      return WAGOSNMP_RETURN_PARSE_OID_ERROR;
    }
    SNMP_MutexUnlock();
    ret = INTERNAL_ConvertTlvToTrapVar(&var, stData);
    if (ret != WAGOSNMP_RETURN_OK) {
      return ret;
    }
    if (NULL == snmp_pdu_add_variable(pdu, OID, OID_length, var.type, var.buf, var.len)) {
      return WAGOSNMP_RETURN_ERR_MALLOC;
    }
  }
  status = snmp_send(ss, pdu) == 0;

  if (status) {
    snmp_sess_perror("snmptrap", ss);
    snmp_free_pdu(pdu);
  }
  snmp_close(ss);
  return ret;
}

INTERNAL_SYM netsnmp_pdu *INTERNAL_GetTrap2PDU_v2_v3(char sEnterprise[128], tWagoSnmpReturnCode *result) {
  netsnmp_pdu *pdu;
  u_long uptime;
  oid entOID[MAX_OID_LEN];
  size_t entOID_length;
  *result = WAGOSNMP_RETURN_OK;

  pdu = snmp_pdu_create(SNMP_MSG_TRAP2);
  if (!pdu) {
    fprintf(stderr, "Failed to create notification PDU\n");
    *result = WAGOSNMP_RETURN_ERR_MALLOC;
    return NULL;
  }

  /*snmp_add_var(pdu, objid_sysuptime,
         sizeof(objid_sysuptime) / sizeof(oid), 't', trap);*/
  uptime = get_uptime();
  if (NULL == snmp_pdu_add_variable(pdu, objid_sysuptime, sizeof(objid_sysuptime) / sizeof(oid), ASN_TIMETICKS, &uptime,
                                    sizeof(uptime))) {
    *result = WAGOSNMP_RETURN_ERR_MALLOC;
    return NULL;
  }

  entOID_length = MAX_OID_LEN;
  SNMP_MutexLock();
  if (!snmp_parse_oid(sEnterprise, entOID, &entOID_length)) {
    SNMP_MutexUnlock();
    snmp_perror(sEnterprise);
    *result = WAGOSNMP_RETURN_PARSE_OID_ERROR;
    return NULL;
  }
  SNMP_MutexUnlock();
  if (NULL == snmp_pdu_add_variable(pdu, objid_snmptrap, sizeof(objid_snmptrap) / sizeof(oid), ASN_OBJECT_ID, entOID,
                                    entOID_length * sizeof(oid))) {
    return NULL;
  }
  return pdu;
}

/* libnetsnmp */

static pthread_mutex_t snmp_mutex;
static pthread_once_t snmp_mutex_init_once = PTHREAD_ONCE_INIT;

static void snmp_mutex_init() {
  pthread_mutexattr_t attr = {0};
  int err                  = pthread_mutexattr_init(&attr);

  error(err, err, "pthread_mutexattr_init");
  err = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
  error(err, err, "pthread_mutexattr_setprotocol");
  err = pthread_mutex_init(&snmp_mutex, &attr);
  error(err, err, "pthread_mutex_init");
}

void SNMP_MutexLock(void) {
  pthread_once(&snmp_mutex_init_once, snmp_mutex_init);
  pthread_mutex_lock(&snmp_mutex);
}

void SNMP_MutexUnlock(void) {
  pthread_mutex_unlock(&snmp_mutex);
}

//---- End of source file ------------------------------------------------------
