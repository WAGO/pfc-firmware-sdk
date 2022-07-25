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
///  \file     wagosnmp_API.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGOSNMP_API_H_
#define WAGOSNMP_API_H_

#include <stdint.h>
#include <stdlib.h>

#define WAGOSNMP_SECURITY_NO_AUTH_NO_PRIV  SNMP_SEC_LEVEL_NOAUTH
#define WAGOSNMP_SECURITY_AUTH_NO_PRIV     SNMP_SEC_LEVEL_AUTHNOPRIV
#define WAGOSNMP_SECURITY_AUTH_PRIV        SNMP_SEC_LEVEL_AUTHPRIV
#define WAGOSNMP_AUTH_P_NONE               0
#define WAGOSNMP_AUTH_P_MD5                1
#define WAGOSNMP_AUTH_P_SHA                2
#define WAGOSNMP_PRIV_P_NONE               0
#define WAGOSNMP_PRIV_P_DES                1
#define WAGOSNMP_PRIV_P_AES                2

#define WAGOSNMP_MAX_STR_LEN 255

#define WAGOSNMP_INTEGER                   ((u_char)0x02)
#define WAGOSNMP_OCTET_STR                 ((u_char)0x04)
#define WAGOSNMP_BIT_STR                   ((u_char)0x03)
#define WAGOSNMP_OBJECT_ID                 ((u_char)0x06)
#define WAGOSNMP_IP_ADDRESS                (((u_char)0x40)|0)
#define WAGOSNMP_COUNTER                   (((u_char)0x40)|1)
#define WAGOSNMP_GAUGE                     (((u_char)0x40) | 2)
#define WAGOSNMP_UNSIGNED                  (((u_char)0x40) | 2)
#define WAGOSNMP_TIMETICKS                 (((u_char)0x40) | 3)
#define WAGOSNMP_COUNTER64                 (((u_char)0x40) | 1) /*same as COUNTER*/
#define WAGOSNMP_NULL                      ((u_char)0x05)

typedef enum {
  WAGOSNMP_RETURN_OK,
  WAGOSNMP_RETURN_INIT_SESSION_ERROR,
  WAGOSNMP_RETURN_BAD_MSG_TYPE,
  WAGOSNMP_RETURN_PARSE_OID_ERROR,
  WAGOSNMP_RETURN_TRANSCEIVE_ERROR,
  WAGOSNMP_RETURN_BAD_DATATYPE,
  WAGOSNMP_RETURN_WARN_STR_VAL_TO_LONG,
  WAGOSNMP_RETURN_ERR_MALLOC,
  WAGOSNMP_RETURN_BAD_STR_VALUE,
  WAGOSNMP_RETURN_ERROR_UNINIT_DATA,
  WAGOSNMP_RETURN_TIMEOUT,
  WAGOSNMP_RETURN_PACKAGE_ERROR,
  WAGOSNMP_RETURN_FB_ERROR,
  WAGOSNMP_RETURN_AUTH_ERR,
  WAGOSNMP_RETURN_PRIV_ERR,
  WAGOSNMP_RETURN_ERROR_BUFFER_TO_SMALL,
  WAGOSNMP_RETURN_ERROR_CONVERT,
  WAGOSNMP_RETURN_ERROR_BAD_ENGINE_ID,
  WAGOSNMP_RETURN_ERROR_INVALID_ENGINE_ID,
  WAGOSNMP_RETURN_ERROR_EXIST,
  WAGOSNMP_RETURN_ERROR_SHM,
  WAGOSNMP_RETURN_NOT_FOUND,
  WAGOSNMP_RETURN_ERROR_PARAMETER,
}tWagoSnmpReturnCode;


typedef uint16_t tWagoSnmpDataType;
typedef uint16_t tWagoSnmpSecLevel;
typedef uint16_t tWagoSnmpAuthProt;
typedef uint16_t tWagoSnmpPrivProt;
typedef uint16_t tWagoSnmpTrapType;


typedef struct stWagoSnmpTlv{char dummy[588];} tWagoSnmpTlv;

typedef enum eMsgType{
  WAGOSNMP_TRCV_GET = 0,
  WAGOSNMP_TRCV_SET,
  WAGOSNMP_TRCV_INFORM,
  WAGOSNMP_TRCV_THREAD_CLOSE
}tWagoSnmpTrcvType;

typedef struct {
        tWagoSnmpTrcvType   trcvType;
        long                version;
        int retries;
        int timeout_us;
        tWagoSnmpTlv *typData; /* VAR_OUTPUT */         /* Structured response data in Type-Length-Value(TLV) format */
        char              * sHost;      /* VAR_INPUT */ /* Hostname or IP-address in "dotted decimal format" */
        size_t              szHost;
        char              * sOID;     /* VAR_INPUT */ /* SNMP unique object identifier(OID) of requested data, e.g. '1.3.6.1.2.1.1.6.0' for "sysLocation"; also used as enterprise OID in case of trap/inform */
        size_t              szOID;
        char              * sCommunity;    /* VAR_INPUT */ /* Community string typical: "public" */
        size_t              szCommunity;
        char              * sEngineId;
        size_t              szEngineId;
        char              * sUsername;    /* VAR_INPUT */ /* SNMP-username. */
        size_t              szUsername;
        tWagoSnmpSecLevel   typSecLevel;      /* VAR_INPUT, Enum: TSNMP_SECURITYLEVEL */
        tWagoSnmpAuthProt   typAuthProt;      /* VAR_INPUT, Enum: TSNMP_AUTHENTICATIONPROTOCOL */
        char              * sAuthPass;    /* VAR_INPUT */ /* Password for the given username */
        size_t              szAuthPass;
        tWagoSnmpPrivProt   typPrivProt;      /* VAR_INPUT, Enum: TSNMP_PRIVACYPROTOCOL */
        char              * sPrivPass;    /* VAR_INPUT */ /* Passphrase for the given encryption type and username */
        size_t              szPrivPass;
        char              * sInformOID;   /*Inform OID used for adding an OID to an Inform*/
        size_t            * szInformOID;
        void              * typDataOld;
}tWagoSnmpTranceiver;



//init will be done implizit
void libwagosnmp_Shutdown(void);
tWagoSnmpReturnCode libwagosnmp_GetErrorString (tWagoSnmpReturnCode code, char * str, size_t szString);
void libwagosnmp_TlvInit (tWagoSnmpTlv  * stTlvData);
void libwagosnmp_TlvDeinit (tWagoSnmpTlv  * stTlvData);
tWagoSnmpDataType libwagosnmp_TlvGetType (tWagoSnmpTlv  * stTlvData);
tWagoSnmpReturnCode libwagosnmp_NullToTlv(tWagoSnmpTlv  * stTlvData);
tWagoSnmpReturnCode libwagosnmp_Int32ToTlv (int32_t input,
                                            tWagoSnmpDataType   eType,
                                            tWagoSnmpTlv  * stData);

tWagoSnmpReturnCode libwagosnmp_Uint32ToTlv(uint32_t            input,
                                            tWagoSnmpDataType   eType,
                                            tWagoSnmpTlv      * stData);
tWagoSnmpReturnCode libwagosnmp_StrToTlv (char              * str,
                                          tWagoSnmpDataType   eType,
                                          tWagoSnmpTlv      * stTlvData);
tWagoSnmpReturnCode libwagosnmp_TlvToInt32 (tWagoSnmpTlv * stTlvData,
                                            int32_t      * input);
tWagoSnmpReturnCode libwagosnmp_TlvToUint32(tWagoSnmpTlv  * stTlvData,
                                            uint32_t      * input);
tWagoSnmpReturnCode libwagosnmp_TlvToStr (tWagoSnmpTlv * stData,
                                          char         * value,
                                          size_t         szValue);
tWagoSnmpReturnCode libwagosnmp_Tranceive (tWagoSnmpTranceiver * trcv);
tWagoSnmpReturnCode libwagosnmp_SendTrap(char * sEnterprise,
                                         tWagoSnmpTrapType trap_type,
                                         uint16_t  specific_type,
                                         char * sOID,
                                         tWagoSnmpTlv * stData);
tWagoSnmpReturnCode libwagosnmp_SendEntTrap(uint16_t specific_type);
tWagoSnmpReturnCode libwagosnmp_SendTrapToAdrV1( char                  * sHost,
                                                 char                  * sCommunity,
                                                 char                  * sEnterprise,
                                                 tWagoSnmpTrapType       trap_type,
                                                 uint16_t                specific_type,
                                                 char                  * sOID,
                                                 tWagoSnmpTlv          * stTlvData);
tWagoSnmpReturnCode libwagosnmp_SendTrapToAdrV2c(char                  * sHost,
                                                 char                  * sCommunity,
                                                 char                  * sEnterprise,
                                                 char                  * sOID,
                                                 tWagoSnmpTlv          * stTlvData);
tWagoSnmpReturnCode libwagosnmp_SendTrapToAdrV3( char                  * sHost,
                                                 char                  * sEngineId,
                                                 char                  * sUsername,
                                                 tWagoSnmpSecLevel       authLevel,
                                                 tWagoSnmpAuthProt       authProt,
                                                 char                  * authPass,
                                                 tWagoSnmpPrivProt       privProt,
                                                 char                  * privPass,
                                                 char                  * sEnterprise,
                                                 char                  * sOID,
                                                 tWagoSnmpTlv          * stTlvData);
tWagoSnmpReturnCode libwagosnmp_RegisterCustomOid(char * sOID,tWagoSnmpTlv * stTlvData,uint8_t readOnly);
tWagoSnmpReturnCode libwagosnmp_GetCustomOid(char * sOID,tWagoSnmpTlv * stTlvData);
tWagoSnmpReturnCode libwagosnmp_SetCustomOid(char * sOID,tWagoSnmpTlv * stTlvData);

#endif /* WAGOSNMP_API_H_ */
//---- End of source file ------------------------------------------------------
