#ifndef D_PBDPM_H
#define D_PBDPM_H

//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 17216 $
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
void dpm_CreateMasterThread(void);
void dpm_ConfigureSubdevice(void);
void dpm_ConfigureDevice(void);
//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define ASYNCHRON_MODE        1
#define SYNCHRON_MODE         2
//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------



typedef struct DPM_BUS_DPtag {
   unsigned short       usBus_Para_Len;   /* length of BUS_DP */
   unsigned char        bFDL_Add;         /* master address */
   unsigned char        bBaudrate;        /* baudrate, see. #defines */
   unsigned short       usTSL;            /* slot-time */
   unsigned short       usMin_TSDR;       /* min. station delay responder */
   unsigned short       usMax_TSDR;       /* max. station delay responder */
   unsigned char        bTQUI;            /* quite time */
   unsigned char        bTSET;            /* setup time */
   unsigned long        ulTTR;            /* target rotation time */
   unsigned char        bG;               /* gap update */
   unsigned char        bHSA;             /* highest active station address */
   unsigned char        bMax_Retry_Limit; /* retries if error occurs */
#define BIT_BP_FLAG_BRESERVED1            0x01
#define BIT_BP_FLAG_BRESERVED2            0x02
#define BIT_BP_FLAG_BRESERVED3            0x04
#define BIT_BP_FLAG_BRESERVED4            0x08
#define BIT_BP_FLAG_BRESERVED5            0x10
#define BIT_BP_FLAG_BRESERVED6            0x20
#define BIT_BP_FLAG_BRESERVED7            0x40
#define BIT_BP_FLAG_BERROR_ACTION_FLAG    0x80
   unsigned char        Bp_Flag;      /* auto_clear on/off */
   unsigned short       usMin_Slave_Intervall; /* min. slave intervall time */
   unsigned short       usPoll_Timeout;        /* poll timeout */
   unsigned short       usData_Control_Time;   /* data control time */
   unsigned char        bMasterSetting:8; /* 0 == big Endian, 1 == little Endian (swapping should be done)*/
   unsigned char        bMax_User_Global_Control; /* Maximum allowed parallel active USER Global Control Commands */

   unsigned char        abOctet[4];
   unsigned short       usMaster_User_Data_Len;
   char                 achMaster_Class2_Name[32];
   unsigned char        abMaster_User_Data[32];
   unsigned long        ulTCL; /* Isochronous cycle time */
   unsigned char        bMax_TSH; /* Maximum Shift Time */
} DPM_BUS_DP;

typedef struct PBMasterDescTag
{
   unsigned short usMasterRefIdDiag          ;
   unsigned long  unMasterBitOffsetDiag         ;
   unsigned char  byAutoStart                ;
   DPM_BUS_DP     dpMaster                ;
   unsigned char  bySlaves                ;
   struct _tagCCModule* pCCModule;
}PBMasterDesc;


/* -------------------------------- */
/* structures to set slave data set */
/* -------------------------------- */
typedef struct PBSlaveHeaderTag
{
   unsigned char     bySlaveStationNumber    ;
//   unsigned short    usSlaveRefIdDiag        ;
//   unsigned long     unSlaveBitOffsetDiag    ;
//   unsigned short    usSlaveRefIdInput       ;
//   unsigned long     unSlaveBitOffsetInput      ;
//   unsigned long     unSlaveInputDataLength     ;
//   unsigned short    usSlaveRefIdOutput         ;
//   unsigned long     unSlaveBitOffsetOutput     ;
//   unsigned long     unSlaveOutputDataLength    ;
}PBSlaveHeader;

/* parameter data header structure of a slave station,
   described in norm EN 50170 page 101 */

typedef struct DPM_SL_PRM_HEADERtag {
  unsigned short  usSlaveParaLen;    /* length of whole parameter data set */
#define BIT_SL_FLAG_BIRESERVED1        0x01
#define BIT_SL_FLAG_BIEXTRA_ALARM_SAP  0x02
#define BIT_SL_FLAG_BIDPV1_DATA_TYPES  0x04
#define BIT_SL_FLAG_BIDPV1_SUPPORTED   0x08
#define BIT_SL_FLAG_BIRESERVED2        0x10
#define BIT_SL_FLAG_BIFAIL_SAFE        0x20
#define BIT_SL_FLAG_BINEW_PRM       0x40
#define BIT_SL_FLAG_BIACTIVE        0x80
  unsigned char      Sl_Flag;          /* slave related flags */
  unsigned char    bSlave_Typ;         /* type of slave */

  unsigned char    bMax_Diag_Data_Len;
  unsigned char    bMax_Alarm_Len;
  unsigned char    bMax_Channel_Data_Length;
  unsigned char    bDiag_Upd_Delay;
#define BIT_ALARM_MODE_BINA_TO_ABORT   0x01
#define BIT_ALARM_MODE_BIIGNORE_ACLR   0x02
#define BIT_ALARM_MODE_ABIRESERVED3    0x04
#define BIT_ALARM_MODE_ABIRESERVED4    0x08
#define BIT_ALARM_MODE_ABIRESERVED5    0x10
#define BIT_ALARM_MODE_ABIRESERVED6    0x20
#define BIT_ALARM_MODE_ABIRESERVED7    0x40
#define BIT_ALARM_MODE_ABIRESERVED8    0x80
  unsigned char    Alarm_Mode;
  unsigned char    bAdd_Sl_Flag;
  unsigned char    abOctet_String[6];

} DPM_SL_PRM_HEADER;

/* parameter data 'Prm_Data' structure of a slave station,
   described in norm EN 50170 page 51 */

#define MAX_USR_PRM_LEN             242

typedef struct DPM_SL_PRM_PRM_DATAtag {
  unsigned short  usPrmDataLen;       /* length of PRM_PRM_DATA */
#define BIT_STATION_STATUS_BRESERVED1    0x01
#define BIT_STATION_STATUS_BRESERVED2    0x02
#define BIT_STATION_STATUS_BRESERVED3    0x04
#define BIT_STATION_STATUS_BWD_ON        0x08
#define BIT_STATION_STATUS_BFREEZE_REQ   0x10
#define BIT_STATION_STATUS_BSYNC_REQ     0x20
#define BIT_STATION_STATUS_BUNLOCK_REQ   0x40
#define BIT_STATION_STATUS_BLOCK_REQ     0x80
  unsigned char   Station_Status; /* status of supported functions */
  unsigned char   bWD_Fact_1;     /* watchdog factor 1 */
  unsigned char   bWD_Fact_2;     /* watchdog factor 2 */
  unsigned char   bMin_Tsdr;      /* min. station delay reponder */
  unsigned short  usIdent_Number; /* ident number of the station, motorola format */
  unsigned char   bGroup_Ident;   /* fixed group number */
  unsigned char   abUsr_Prm_Data[MAX_USR_PRM_LEN];
} DPM_SL_PRM_PRM_DATA;


/* configuration data 'Cfg_Data' structure of a slave station,
   described in norm EN 50170 page 55 */
#define MAX_CFG_DATA_LEN 255

typedef struct DPM_SL_PRM_CFG_DATAtag {
  unsigned short  usCfg_Data_Len;  /* length of CFG_DATA */
  unsigned char   abCfg_Data[MAX_CFG_DATA_LEN]; /* configuration data of the
                                                   slave station */
} DPM_SL_PRM_CFG_DATA;


/* i/o offset table 'Add_Tab' structure of a slave station,
   described in norm EN 50170 page 103 */
#define MAX_EA_OFFSET_LEN 244

typedef struct DPM_SL_PRM_ADD_TABtag {
  unsigned short  usAdd_Tab_Len; /* length of ADD_TAB */
  unsigned char   bInput_Count;  /* counter of following input offsets */
  unsigned char   bOutput_Count; /* counter of following ouput offsets */
  unsigned short  ausEA_Offset[MAX_EA_OFFSET_LEN]; /* user defined offsets */
} DPM_SL_PRM_ADD_TAB;


/* extented data 'Slave_User_Data' structure of a slave station,
   described in norm EN 50170 page 103 */
#define  MAX_SL_PRM_LEN  100

typedef struct DPM_SL_PRM_USR_DATAtag {
  unsigned short usSlave_Usr_Data_Len; /* length of USR_DATA */
  unsigned char  abSlave_Usr_Data[MAX_SL_PRM_LEN]; /* user paramteres */
} DPM_SL_PRM_USR_DATA;

typedef struct PBSlaveTag
{
   PBSlaveHeader        Header;
   DPM_SL_PRM_HEADER    slHeader;
   DPM_SL_PRM_PRM_DATA  slPrmData;
   DPM_SL_PRM_CFG_DATA  slCfgData;

}PBSlave;


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


#endif  // D_PBDPM_H
