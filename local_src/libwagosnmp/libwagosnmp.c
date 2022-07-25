//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "wagosnmp_API.h"
#include "wagosnmp_internal.h"

PUBLIC_SYM void init_libwagosnmp_AgentEntry(void);

/* init_function for snmpd-plugin */
void init_libwagosnmp_AgentEntry(void)
{
    snmp_alarm_register(3,0, AGENT_InitServerCommunication, NULL);
}

void libwagosnmp_Shutdown(void)
{
  snmp_shutdown(WAGOSNMP_INIT_NAME);
  INTERNAL_ResetSnmpAgent();
  snmp_is_initialized = PTHREAD_ONCE_INIT;
  snmp_agent_is_initialized = PTHREAD_ONCE_INIT;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_GetErrorString (tWagoSnmpReturnCode code, char * str, size_t szString)
{
  typedef struct {
      tWagoSnmpReturnCode code;
      char const * string;
  }tRetToStr;
  tWagoSnmpReturnCode ret=WAGOSNMP_RETURN_OK;
#define RET_TO_STRING(x) {.code=x, .string=#x}
  static const tRetToStr strings[] = {
                     RET_TO_STRING(WAGOSNMP_RETURN_OK),
                     RET_TO_STRING(WAGOSNMP_RETURN_INIT_SESSION_ERROR),
                     RET_TO_STRING(WAGOSNMP_RETURN_BAD_MSG_TYPE),
                     RET_TO_STRING(WAGOSNMP_RETURN_PARSE_OID_ERROR),
                     RET_TO_STRING(WAGOSNMP_RETURN_TRANSCEIVE_ERROR),
                     RET_TO_STRING(WAGOSNMP_RETURN_BAD_DATATYPE),
                     RET_TO_STRING(WAGOSNMP_RETURN_WARN_STR_VAL_TO_LONG),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERR_MALLOC),
                     RET_TO_STRING(WAGOSNMP_RETURN_BAD_STR_VALUE),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_UNINIT_DATA),
                     RET_TO_STRING(WAGOSNMP_RETURN_TIMEOUT),
                     RET_TO_STRING(WAGOSNMP_RETURN_PACKAGE_ERROR),
                     RET_TO_STRING(WAGOSNMP_RETURN_FB_ERROR),
                     RET_TO_STRING(WAGOSNMP_RETURN_AUTH_ERR),
                     RET_TO_STRING(WAGOSNMP_RETURN_PRIV_ERR),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_BUFFER_TO_SMALL),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_CONVERT),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_BAD_ENGINE_ID),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_INVALID_ENGINE_ID),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_INVALID_ENGINE_ID),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_EXIST),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_SHM),
                     RET_TO_STRING(WAGOSNMP_RETURN_NOT_FOUND),
                     RET_TO_STRING(WAGOSNMP_RETURN_ERROR_PARAMETER)
  };
  size_t i;
  char unknown[]= "UNKNOWN";
  char const * pStr = unknown;
  for(i=0;i < sizeof(strings)/sizeof(tRetToStr);i++)
  {
    if(strings[i].code == code)
    {
      pStr = strings[i].string;
      break;
    }
  }

  i = strlen(pStr) + 1;
  if(szString < i)
  {
    ret = WAGOSNMP_RETURN_WARN_STR_VAL_TO_LONG;
  }
  strncpy(str,pStr,szString);
  str[szString-1]=0;
  return ret;

}

PUBLIC_SYM void libwagosnmp_TlvInit (tWagoSnmpTlv  * stTlvData)
{
  memset(stTlvData, 0, sizeof(netsnmp_variable_list));
}

PUBLIC_SYM void libwagosnmp_TlvDeinit (tWagoSnmpTlv  * stTlvData)
{
  (void) libwagosnmp_NullToTlv(stTlvData);
}

PUBLIC_SYM tWagoSnmpDataType libwagosnmp_TlvGetType (tWagoSnmpTlv  * stTlvData)
{
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  return (tWagoSnmpDataType)stData->type;
}

//-- Function: PLCSNMP_MANAGER_null_to_tlv -----------------------------------------
///
///  PLC-Function SNMPM_NULL_TO_TLV
///
///
///
///   FUNCTION SNMPM_NULL_TO_TLV : INT
///   (* This function convert a given ULINT value into the TLV-structure
///
///     Return values:
///       0: Successful executed
///
///       7 : Not enough memory on device
///   *)
///   VAR_IN_OUT
///     stData  : tSNMPM_TLV;       (* Container for TLV *)
///   END_VAR
///   VAR
///   END_VAR
///
///  Body:
///   ;
//-----------------------------------------------------------------------------
PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_NullToTlv(tWagoSnmpTlv  * stTlvData)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  u_char null=0;
  INTERNAL_ReTwist(stData);
  if(INTERNAL_SetVarTypedValue  (stData,ASN_NULL,&null,sizeof(null)))
  {
    ret = WAGOSNMP_RETURN_ERR_MALLOC;
  }
  INTERNAL_DeTwist(stData);
  return ret;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_Int32ToTlv (int32_t input,
                                                       tWagoSnmpDataType   eType,
                                                       tWagoSnmpTlv  * stTlvData)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;

  if(eType != ASN_INTEGER)
  {
    return WAGOSNMP_RETURN_BAD_DATATYPE;
  }

  INTERNAL_ReTwist(stData);
  if(INTERNAL_SetVarTypedValue  (stData,(u_char)eType,(u_char*)&input,sizeof(int)))
  {
    ret = WAGOSNMP_RETURN_ERR_MALLOC;
  }
  INTERNAL_DeTwist(stData);
  return ret;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_Uint32ToTlv(uint32_t            input,
                                                       tWagoSnmpDataType   eType,
                                                       tWagoSnmpTlv      * stTlvData)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;

  if(   (eType != ASN_UNSIGNED)
      &&(eType != ASN_TIMETICKS)
      &&(eType != ASN_COUNTER)
      &&(eType != ASN_UINTEGER))
  {
    return WAGOSNMP_RETURN_BAD_DATATYPE;
  }

  INTERNAL_ReTwist(stData);
  if(INTERNAL_SetVarTypedValue  (stData,(u_char)eType,(u_char*)&input,sizeof(uint32_t)))
  {
    ret = WAGOSNMP_RETURN_ERR_MALLOC;
  }
  INTERNAL_DeTwist(stData);
  return ret;
}

//-- Function: PLCSNMP_MANAGER_str_to_tlv -----------------------------------------
///
///  PLC-Function SNMPM_STRING_TO_TLV
///  try to convert all given STRING-Values into the given data-type
///
//-----------------------------------------------------------------------------
PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_StrToTlv (char              * str,
                                                     tWagoSnmpDataType   eType,
                                                     tWagoSnmpTlv      * stTlvData)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  u_char val_buf[WAGOSNMP_MAX_STR_LEN+1];
  u_char * str_val = val_buf;
  size_t val_len=0;

  oid    anOID[MAX_OID_LEN];
  INIT_SNMP_ONCE;
  INTERNAL_ReTwist(stData);

  switch(eType)
  {
    case ASN_INTEGER:
    case ASN_UNSIGNED:
    case ASN_TIMETICKS:
    case ASN_COUNTER:
    case ASN_UINTEGER:
    {
      char * endptr = NULL;
      if(eType == ASN_INTEGER)
      {
        int * pInt=(int*)str_val;
        errno = 0;
        *pInt = strtol(str, &endptr, 0);
      }
      else
      {
        unsigned int * pUint=(unsigned int*)str_val;
        errno = 0;
        *pUint = strtoul(str, &endptr, 0);
      }
      if(   (errno != 0)
          ||(endptr == str))
      {
        ret = WAGOSNMP_RETURN_BAD_STR_VALUE;
      }
      val_len=sizeof(int);
    }
      break;
    case ASN_OBJECT_ID:
    case ASN_PRIV_IMPLIED_OBJECT_ID:
    case ASN_PRIV_INCL_RANGE:
    case ASN_PRIV_EXCL_RANGE:
    {
      size_t anOID_len = MAX_OID_LEN;
      SNMP_MutexLock();
      if (!snmp_parse_oid(str, anOID, &anOID_len))
      {
        ret = WAGOSNMP_RETURN_BAD_STR_VALUE;
      }
      SNMP_MutexUnlock();
      str_val = (u_char*)anOID;
      val_len = anOID_len * (sizeof(oid));
      break;
    }
    case ASN_IPADDRESS: /* snmp_build_var_op treats IPADDR like a string */
    {
      int IPaddr[4];
      int i;
      if(4 == sscanf(str, "%d.%d.%d.%d",
                    &IPaddr[0], &IPaddr[1], &IPaddr[2], &IPaddr[3]))

      {
        for(i = 0; i < 4; i++)
        {
          str_val[i] = (u_char)IPaddr[i];
          val_len = 4;
        }
      }
      else
      {
        ret = WAGOSNMP_RETURN_BAD_STR_VALUE;
      }
      break;
    }
    case ASN_PRIV_IMPLIED_OCTET_STR:
    case ASN_OCTET_STR:
    case ASN_BIT_STR:
    case ASN_OPAQUE:
    case ASN_NSAP:
      str_val = (u_char*)str;
      val_len = strlen(str);
      break;
    case ASN_COUNTER64:
    {
      unsigned long long int * pUllint=(unsigned long long int*)str_val;
      char * endptr = NULL;
      errno = 0;
      *pUllint = strtoull(str, &endptr, 0);
      if(   (errno != 0)
          ||(endptr == str))
      {
        ret = WAGOSNMP_RETURN_BAD_STR_VALUE;
      }
      val_len=sizeof(unsigned long long int);
      break;
    }
    default:
      ret = WAGOSNMP_RETURN_BAD_DATATYPE;
      break;
  }
  if(ret == WAGOSNMP_RETURN_OK)
  {
    if(INTERNAL_SetVarTypedValue  (stData,(u_char)eType,str_val,val_len))
    {
      ret = WAGOSNMP_RETURN_ERR_MALLOC;
    }
  }

  INTERNAL_DeTwist(stData);

  return ret;
}

//-- Function: PLCSNMP_MANAGER_tlv_to_dint -----------------------------------------
///
///  PLC-Function SNMPM_TLV_TO_DINT
///
//-----------------------------------------------------------------------------
PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_TlvToInt32 (tWagoSnmpTlv  * stTlvData, int32_t * input)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  INTERNAL_ReTwist(stData);
  if(stData->type == ASN_INTEGER)
  {
    *input = *(stData->val.integer);
  }
  else
  {
    ret = WAGOSNMP_RETURN_BAD_DATATYPE;
  }
  INTERNAL_DeTwist(stData);
  return ret;
}

//-- Function: PLCSNMP_MANAGER_tlv_to_udint -----------------------------------------
///
///  PLC-Function SNMPM_TLV_TO_DINT
///
//-----------------------------------------------------------------------------
PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_TlvToUint32(tWagoSnmpTlv  * stTlvData,
                                                       uint32_t      * input)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  INTERNAL_ReTwist(stData);
  if(   (stData->type == ASN_UNSIGNED)
      ||(stData->type == ASN_TIMETICKS)
      ||(stData->type == ASN_COUNTER)
      ||(stData->type == ASN_UINTEGER))
  {
    *input = (uint32_t)*(stData->val.integer);
  }
  else
  {
    ret = WAGOSNMP_RETURN_BAD_DATATYPE;
  }
  INTERNAL_DeTwist(stData);
  return ret;
}

//-- Function: PLCSNMP_MANAGER_tlv_to_str -----------------------------------------
///
///  PLC-Function SNMPM_TLV_TO_STRING
///
//-----------------------------------------------------------------------------
PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_TlvToStr (tWagoSnmpTlv  * stTlvData,
                                                     char * value,
                                                     size_t szValue)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;

  size_t len = stData->val_len;
  char buf[WAGOSNMP_MAX_STR_LEN+64];
  if(len == 0)
  {
    return WAGOSNMP_RETURN_ERROR_UNINIT_DATA;
  }
  if(len > WAGOSNMP_MAX_STR_LEN)
  {
    ret = WAGOSNMP_RETURN_WARN_STR_VAL_TO_LONG;
    len = WAGOSNMP_MAX_STR_LEN;
  }
  if(len > szValue)
  {
    return WAGOSNMP_RETURN_ERROR_BUFFER_TO_SMALL;
  }
  INTERNAL_ReTwist(stData);
  if(-1 == INTERNAL_SnprintValue(buf, sizeof(buf), stData->name, stData->name_length, stData))
  {
    ret = WAGOSNMP_RETURN_BAD_DATATYPE;
  }
  else
  {
    char * p = strchr(buf, ':');

    if(p != NULL)
    {
      size_t bufsiz;
      p++;
      p++;
      if(stData->type == ASN_OCTET_STR)
      {
        p = INTERNAL_StripQuotes(p);
      }
      bufsiz = (size_t)(p-buf);
      if(bufsiz <= (sizeof(buf)-WAGOSNMP_MAX_STR_LEN))
      {
        bufsiz =  WAGOSNMP_MAX_STR_LEN;
      }
      else
      {
        bufsiz = WAGOSNMP_MAX_STR_LEN - bufsiz;
      }
      memcpy(value, p, bufsiz-1);
      value[bufsiz]=0;
    }
    else
    {
      ret = WAGOSNMP_RETURN_ERROR_CONVERT;
    }
  }
  INTERNAL_DeTwist(stData);
  return ret;
}


PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_Tranceive(tWagoSnmpTranceiver * trcv)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_session * ss = NULL;
  netsnmp_pdu     * pdu = NULL;

  do {
    ret = INTERNAL_GetSnmpSession(trcv, &ss);
    if(ret != WAGOSNMP_RETURN_OK)break;
    ret = INTERNAL_GetSnmpPdu(trcv, &pdu);
    if(ret != WAGOSNMP_RETURN_OK)
    {
      snmp_close(ss);
      break;
    }
    ret = INTERNAL_Tranceive(trcv, pdu, ss);
  }while(0);

  return ret;
}


PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_SendTrap(char * sEnterprise,
                                                    tWagoSnmpTrapType trap_type,
                                                    uint16_t  specific_type,
                                                    char * sOID,
                                                    tWagoSnmpTlv * stTlvData)
{
  tWagoSnmpReturnCode ret=WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  tWagoSnmpMsg trap;

  INIT_SNMP_ONCE;
  trap.enterprise_length = MAX_OID_LEN;
  SNMP_MutexLock();
  if (!snmp_parse_oid(sEnterprise, trap.enterprise, &trap.enterprise_length)) {
    SNMP_MutexUnlock();
    snmp_perror(sEnterprise);
    return WAGOSNMP_RETURN_PARSE_OID_ERROR;
  }

  if(    (sOID[0]!=0)
      && (stData->type != ASN_NULL ))
  {

  trap.variable.sOID_length = MAX_OID_LEN;
  if (!snmp_parse_oid(sOID, trap.variable.sOID, &trap.variable.sOID_length)) {
      SNMP_MutexUnlock();
      snmp_perror(sOID);
    return WAGOSNMP_RETURN_PARSE_OID_ERROR;
  }
  SNMP_MutexUnlock();
  ret=INTERNAL_ConvertTlvToTrapVar(&trap.variable,stData);
  }
  else
  {
    trap.variable.type=ASN_NULL;
  }

  trap.type = MSG_TYPE_TRAP;
  trap.trap_type = trap_type;
  trap.specific_type = specific_type;

  if(0 != INTERNAL_SendTrapMsg(&trap))
  {
    ret = WAGOSNMP_RETURN_TRANSCEIVE_ERROR;
  }
  //snmp_close(&session);
  return ret;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_SendEntTrap(uint16_t specific_type)
{
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  tWagoSnmpMsg easyTrap;

  /*if(specific_type < 25)
  {
    return WAGOSNMP_RETURN_ERROR_PARAMETER;
  }*/

  easyTrap.type = MSG_TYPE_TRAP_EASY;
  easyTrap.specific_type = (long int) specific_type;

 // INIT_SNMP_ONCE;
  if(0!=INTERNAL_SendTrapMsg(&easyTrap))
  {
    ret=WAGOSNMP_RETURN_TRANSCEIVE_ERROR;
    perror("Error Send Trap:");
  }
  return ret;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_SendTrapToAdrV1( char                  * sHost,
                                                            char                  * sCommunity,
                                                            char                  * sEnterprise,
                                                            tWagoSnmpTrapType       trap_type,
                                                            uint16_t                specific_type,
                                                            char                  * sOID,
                                                            tWagoSnmpTlv          * stTlvData)
{
  netsnmp_session *ss;
  netsnmp_pdu    *pdu;
  oid             entOID[MAX_OID_LEN];
  size_t          entOID_length;
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  in_addr_t *myaddr;

  if(   (trap_type > 6)
     /*|| (specific_type < 25 && trap_type == 6)*/)
  {
    return WAGOSNMP_RETURN_ERROR_PARAMETER;
  }

   ss=INTERNAL_GenerateSession_v1_v2c(sHost,sCommunity,SNMP_VERSION_1);
   if(ss == NULL)
   {
     return WAGOSNMP_RETURN_INIT_SESSION_ERROR;
   }

   pdu = snmp_pdu_create(SNMP_MSG_TRAP);
   if ( !pdu ) {
     fprintf(stderr, "Failed to create trap PDU\n");
     return WAGOSNMP_RETURN_ERR_MALLOC;
   }
   entOID_length = MAX_OID_LEN;
   SNMP_MutexLock();
   if (!snmp_parse_oid(sEnterprise, entOID, &entOID_length)) {
     SNMP_MutexUnlock();
     snmp_perror(sEnterprise);
     return WAGOSNMP_RETURN_PARSE_OID_ERROR;
   }
   SNMP_MutexUnlock();
   pdu->enterprise = (oid *) malloc(entOID_length * sizeof(oid));
   memcpy(pdu->enterprise, entOID, entOID_length * sizeof(oid));
   pdu->enterprise_length = entOID_length;

   pdu->trap_type = trap_type;
   pdu->specific_type = specific_type;

   myaddr = (in_addr_t *) pdu->agent_addr;
   *myaddr = get_myaddr();
   pdu->time = get_uptime();

   ret = INTERNAL_AddVarAndSend(sOID,stData,ss,pdu);

   return ret;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_SendTrapToAdrV2c(char                  * sHost,
                                                            char                  * sCommunity,
                                                            char                  * sEnterprise,
                                                            char                  * sOID,
                                                            tWagoSnmpTlv          * stTlvData)
{
  netsnmp_session *ss;
  netsnmp_pdu    *pdu;
  tWagoSnmpReturnCode ret = WAGOSNMP_RETURN_OK;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;

   ss=INTERNAL_GenerateSession_v1_v2c(sHost,sCommunity,SNMP_VERSION_2c);
   if(ss == NULL)
   {
     return WAGOSNMP_RETURN_INIT_SESSION_ERROR;
   }

   pdu =INTERNAL_GetTrap2PDU_v2_v3(sEnterprise,&ret);
   if(ret == WAGOSNMP_RETURN_OK)
   {
     ret = INTERNAL_AddVarAndSend(sOID,stData,ss,pdu);
   }

   return ret;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_SendTrapToAdrV3(char                  * sHost,
                                                           char                  * sEngineId,
                                                           char                  * sUsername,
                                                           tWagoSnmpSecLevel       authLevel,
                                                           tWagoSnmpAuthProt       authProt,
                                                           char                  * authPass,
                                                           tWagoSnmpPrivProt       privProt,
                                                           char                  * privPass,
                                                           char                  * sEnterprise,
                                                           char                  * sOID,
                                                           tWagoSnmpTlv          * stTlvData)
{

  netsnmp_session *ss;
  netsnmp_pdu    *pdu;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;

  tWagoSnmpTranceiver trcv;

  netsnmp_session tmpss;
  netsnmp_transport * transport;
  tWagoSnmpReturnCode result;

  INIT_SNMP_ONCE;
  snmp_sess_init( &tmpss );

  tmpss.version = SNMP_VERSION_3;
  tmpss.peername = sHost;


  tmpss.securityName = sUsername; //s.o.
  tmpss.securityNameLen = strlen(tmpss.securityName);

  /* set the security level to authenticated, but not encrypted */
  tmpss.securityLevel = authLevel;
  trcv.typSecLevel=authLevel;
  trcv.typAuthProt=authProt;
  trcv.sAuthPass=authPass;
  trcv.typPrivProt=privProt;
  trcv.sPrivPass=privPass;
  INTERNAL_SetAuthPriv(&trcv,&tmpss);

  tmpss.callback = INTERNAL_SnmpInput;
  tmpss.callback_magic = NULL;

  setup_engineID(NULL, NULL);
  if (tmpss.contextEngineIDLen == 0 ||
    tmpss.contextEngineID == NULL)
  {
    tmpss.contextEngineID = snmpv3_generate_engineID(&tmpss.contextEngineIDLen);
  }

  if(sEngineId != NULL && sEngineId[0] != 0)
  {
    size_t ebuf_len = 32, eout_len = 0;
    u_char *ebuf = (u_char *)malloc(ebuf_len);

    if (!snmp_hex_to_binary(&ebuf, &ebuf_len, &eout_len, 1, sEngineId))
    {
      fprintf(stderr, "Bad engine ID value.\n");
      free(ebuf);
      return WAGOSNMP_RETURN_ERROR_BAD_ENGINE_ID;
    }
    if ((eout_len < 5) || (eout_len > 32))
    {
      fprintf(stderr, "Invalid engine ID value\n");
      free(ebuf);
      return WAGOSNMP_RETURN_ERROR_INVALID_ENGINE_ID;
    }
    tmpss.securityEngineID = ebuf;
    tmpss.securityEngineIDLen = eout_len;
  }

  if (tmpss.securityEngineIDLen == 0 ||
      tmpss.securityEngineID == NULL)
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

  transport = netsnmp_transport_open_client("snmptrap", tmpss.peername);
  if(transport == NULL)
  {
    result = WAGOSNMP_RETURN_INIT_SESSION_ERROR;
  }
  else
  {
    ss = snmp_add(&tmpss,
                  transport,
                  NULL, NULL);

    pdu =INTERNAL_GetTrap2PDU_v2_v3(sEnterprise,&result);
    if(result == WAGOSNMP_RETURN_OK)
    {
      result = INTERNAL_AddVarAndSend(sOID,stData,ss,pdu);
    }
  }

  return  result;

}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_RegisterCustomOid(char * sOID,tWagoSnmpTlv * stTlvData,uint8_t readOnly)
{
  tWagoSnmpReturnCode result = WAGOSNMP_RETURN_ERROR_EXIST;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  size_t anOID_len = MAX_OID_LEN;
  oid anOID[MAX_OID_LEN];

  if(!SNMP_TLV_INITALIZED(stTlvData) || SNMP_TLV_IS_NULL(stTlvData))
  {
    return WAGOSNMP_RETURN_ERROR_PARAMETER;
  }

  INIT_SNMP_AGENT_ONCE;
  INTERNAL_ReTwist(stData);
  SNMP_MutexLock();
  if (!snmp_parse_oid(sOID, anOID, &anOID_len))
  {
    SNMP_MutexUnlock();
    snmp_perror(sOID);
    return WAGOSNMP_RETURN_PARSE_OID_ERROR;
  }
  SNMP_MutexUnlock();

  if(AGENT_GetOidObject(anOID,anOID_len) == NULL)
  {
    result = AGENT_CreateNewOidObject(anOID,anOID_len,stData,readOnly);
  }
  if(result == WAGOSNMP_RETURN_OK)
  {
    result = INTERNAL_InformForNewOid(AGENT_GetOidObject(anOID,anOID_len));
  }
  INTERNAL_DeTwist(stData);
  return result;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_GetCustomOid(char * sOID,tWagoSnmpTlv * stTlvData)
{
  tWagoSnmpReturnCode result = WAGOSNMP_RETURN_NOT_FOUND;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  size_t anOID_len = MAX_OID_LEN;
  oid anOID[MAX_OID_LEN];
  tOidObject * object = NULL;

  INIT_SNMP_AGENT_ONCE;
  SNMP_MutexLock();
  if (!snmp_parse_oid(sOID, anOID, &anOID_len))
  {
    SNMP_MutexUnlock();
    snmp_perror(sOID);
    return WAGOSNMP_RETURN_PARSE_OID_ERROR;
  }
  SNMP_MutexUnlock();

  INTERNAL_ReTwist(stData);
  AGENT_MutexLock();
  object = AGENT_GetOidObject(anOID,anOID_len);
  if( object != NULL)
  {

    if(object->type > UINT8_MAX
      || INTERNAL_SetVarTypedValue(stData,(u_char) object->type,object->buf,object->len))
    {
      result = WAGOSNMP_RETURN_ERR_MALLOC;
    }
    else
    {
      result = WAGOSNMP_RETURN_OK;
    }
  }
  AGENT_MutexUnlock();
  INTERNAL_DeTwist(stData);
  return result;
}

PUBLIC_SYM tWagoSnmpReturnCode libwagosnmp_SetCustomOid(char * sOID,tWagoSnmpTlv * stTlvData)
{
  tWagoSnmpReturnCode result = WAGOSNMP_RETURN_NOT_FOUND;
  netsnmp_variable_list * stData = (netsnmp_variable_list*)stTlvData;
  size_t anOID_len = MAX_OID_LEN;
  oid anOID[MAX_OID_LEN];
  tOidObject * object = NULL;

  if(!SNMP_TLV_INITALIZED(stTlvData) || SNMP_TLV_IS_NULL(stTlvData))
  {
    return WAGOSNMP_RETURN_ERROR_PARAMETER;
  }

  INIT_SNMP_AGENT_ONCE;
  SNMP_MutexLock();
  if (!snmp_parse_oid(sOID, anOID, &anOID_len))
  {
    SNMP_MutexUnlock();
    snmp_perror(sOID);
    return WAGOSNMP_RETURN_PARSE_OID_ERROR;
  }
  SNMP_MutexUnlock();

  INTERNAL_ReTwist(stData);
  AGENT_MutexLock();
  object = AGENT_GetOidObject(anOID,anOID_len);
  if( object != NULL)
  {
    AGENT_SetOidObjectValue(object,stData);
    result = WAGOSNMP_RETURN_OK;
  }
  AGENT_MutexUnlock();
  INTERNAL_DeTwist(stData);
  return result;
}
