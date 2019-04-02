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
///  \file     device_profibus_master.c
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <adi_application_interface.h>
#include <dpm_demo.h>
#include <menu.h>
#include <device_type.h>

#include "profibus_master.h"




#define ICM_UNUSED_TASK_ID         0x55
#define MAX_SLAVE                  2

tBatchProcessing *batchData;


  PBSlave  slave_table[MAX_SLAVE] = {
                                      {    /* Slave #6 750-333 */
            /* slave address */         {6},                                                \
                                        {0x44, 0xAC, 0x3, 0x80, 0, 0xF0, 0, 0, 0},          \
                                        {0x25, 0x88, 0x1, 0x64, 0xb, 0x54B7, 0,             \
                                         {0xc0, 0, 0, 0, 0, 0, 0, 0x2, 0, 0xC3, 0x43, 0,    \
                                          0, 0, 0x1, 0x4, 0, 0, 0,                          \
                                          0x80, 0x21, 0, 0x23, 0x1, 0, 0x23, 0x2, 0}        \
                                        },                                                  \
                                        { 0x5, {0, 0x10, 0x20} }                            \
                                      },
                                      {  /* Slave #4 750-833*/
          /* slave address */           {4},                                                \
                                        {0x44, 0xAC, 0x3, 0x80, 0, 0xF0, 0, 0, 0},          \
                                        {0x25, 0x88, 0x1, 0x64, 0xB, 0x56B7, 0,             \
                                         {0xc0, 0, 0, 0, 0, 0, 0, 0x2, 0, 0xC3, 0x43, 0xC3, \
                                          0x7F, 0, 0x1, 0x4, 0, 0, 0,                       \
                                          0x80, 0x21, 0, 0x23, 0x1, 0, 0x23, 0x2, 0}        \
                                        },                                                  \
                                        { 0x5, {0, 0x10, 0x20} }                            \
                                       }
                                    };

  DPM_BUS_DP master_table = { 0x42, 0x1, 0x6, 0x190, 0xB, 0x96, 0, 0x1, 0x1161, 0xA, 0x3, 0x2, 0, 0x5, 0xA, 0x4B0, 0};





void *dpmThread(void *arg)
{
  batchData                 = menu_GetBatchData();
  static uint8_t dpm_inpData[1000]      = {0};
  static uint8_t dpm_outpData[1000]      = {0};
  static uint8_t processOutpData[1000]      = {0};
  int32_t deviceId;
  uint8_t i;
  int32_t adiResponse               = DAL_SUCCESS;
  UNUSED_PARAMETER(arg);

  deviceId    = device_getIdFromDeviceName("libpbdpm");

  processOutpData[0] = dpm_outpData[0] = 0xaa;


  while(batchData->cancel == false)
  {

    for(i = 0; i < MAX_SLAVE; i++)
    {
      if(dpm_inpData[i] ==  processOutpData[i])
      {
        processOutpData[i] ++;
      }

      dpm_outpData[i]= processOutpData[i];

      /*    write source data into destination device  */
                    // lock write device
      adiResponse |= adi_GetApplicationInterface()->WriteStart(deviceId, ICM_UNUSED_TASK_ID);

      // write data into destination device
      adiResponse |= adi_GetApplicationInterface()->WriteBytes(deviceId, ICM_UNUSED_TASK_ID, i, 1, &dpm_outpData[i]);
                    // unlock write device
      adiResponse |= adi_GetApplicationInterface()->WriteEnd(deviceId, ICM_UNUSED_TASK_ID);

      usleep(10000);         //10ms

          // lock read device
      adiResponse |= adi_GetApplicationInterface()->ReadStart(deviceId, ICM_UNUSED_TASK_ID);
         // read data from source device
      adiResponse |= adi_GetApplicationInterface()->ReadBytes(deviceId, ICM_UNUSED_TASK_ID, i, 1, &dpm_inpData[i]);
         // unlock read device
      adiResponse |= adi_GetApplicationInterface()->ReadEnd(deviceId, ICM_UNUSED_TASK_ID);

      usleep(10000);         //10ms
    }
  }

  return NULL;
}



void dpm_ConfigureSubdevice(void)
{

  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  int32_t deviceId;
  int32_t subDeviceId = 0;
  uint8_t i;
  uint8_t n;
  void *configData;
  static uint8_t inputCount = 0;
  static uint8_t outputCount = 0;

  deviceId    = device_getIdFromDeviceName("libpbdpm");

  uint8_t slaveParameter[1000] = {0};
  PBSlave      profibus_config;


   uint32_t size;

  for(i= 0; i < MAX_SLAVE; i++)
  {
      /* clear config buffer */
    memset(&profibus_config, 0, sizeof(profibus_config));
    memset(&slaveParameter, 0, sizeof(slaveParameter));
    configData = &slaveParameter[0];

    subDeviceId                                       = slave_table[i].Header.bySlaveStationNumber;
    profibus_config.slHeader.usSlaveParaLen           = slave_table[i].slHeader.usSlaveParaLen;
    profibus_config.slHeader.Sl_Flag                  = slave_table[i].slHeader.Sl_Flag;
    profibus_config.slHeader.bSlave_Typ               = slave_table[i].slHeader.bSlave_Typ;
    profibus_config.slHeader.bMax_Diag_Data_Len       = slave_table[i].slHeader.bMax_Diag_Data_Len;
    profibus_config.slHeader.bMax_Alarm_Len           = slave_table[i].slHeader.bMax_Alarm_Len;
    profibus_config.slHeader.bMax_Channel_Data_Length = slave_table[i].slHeader.bMax_Channel_Data_Length;
    profibus_config.slHeader.bDiag_Upd_Delay          = slave_table[i].slHeader.bDiag_Upd_Delay;
    profibus_config.slHeader.Alarm_Mode               = slave_table[i].slHeader.Alarm_Mode;
    profibus_config.slHeader.bAdd_Sl_Flag             = slave_table[i].slHeader.bAdd_Sl_Flag;

    memcpy(&slaveParameter[0], &profibus_config.slHeader, sizeof(profibus_config.slHeader));
    size = sizeof(profibus_config.slHeader);

    profibus_config.slPrmData.usPrmDataLen            = slave_table[i].slPrmData.usPrmDataLen;
    profibus_config.slPrmData.Station_Status          = slave_table[i].slPrmData.Station_Status;
    profibus_config.slPrmData.bWD_Fact_1              = slave_table[i].slPrmData.bWD_Fact_1;
    profibus_config.slPrmData.bWD_Fact_2              = slave_table[i].slPrmData.bWD_Fact_2;
    profibus_config.slPrmData.bMin_Tsdr               = slave_table[i].slPrmData.bMin_Tsdr;
    profibus_config.slPrmData.usIdent_Number          = slave_table[i].slPrmData.usIdent_Number;
    profibus_config.slPrmData.bGroup_Ident            = slave_table[i].slPrmData.bGroup_Ident;

    for(n=0; n<profibus_config.slPrmData.usPrmDataLen-9; n++)
    {
      profibus_config.slPrmData.abUsr_Prm_Data[n] = slave_table[i].slPrmData.abUsr_Prm_Data[n];
    }
    memcpy(&slaveParameter[size], &profibus_config.slPrmData, profibus_config.slPrmData.usPrmDataLen);
    size += profibus_config.slPrmData.usPrmDataLen;

    profibus_config.slCfgData.usCfg_Data_Len          = slave_table[i].slCfgData.usCfg_Data_Len;
    for(n=0; n < profibus_config.slCfgData.usCfg_Data_Len-2; n++)
    {
      profibus_config.slCfgData.abCfg_Data[n] = slave_table[i].slCfgData.abCfg_Data[n];
    }
    memcpy(&slaveParameter[size], &profibus_config.slCfgData, profibus_config.slCfgData.usCfg_Data_Len);
    size += profibus_config.slCfgData.usCfg_Data_Len;

                                // ADD TAB
    slaveParameter[size++]=8;
    slaveParameter[size++]=0;
    slaveParameter[size++]=1;
    slaveParameter[size++]=1;
    slaveParameter[size++]=inputCount++;
    slaveParameter[size++]=0x80;
    slaveParameter[size++]=outputCount++;
    slaveParameter[size++]=0x80;
                                // Slave user data
    slaveParameter[size++]=2;
    slaveParameter[size++]=0;
    slaveParameter[size]=0;



    adiResponse = adi->ConfigureSubdevice(deviceId, subDeviceId, configData);

    if(adiResponse != DAL_SUCCESS)
    {
      printf("Slave Configuration failed.\n");
      return;
    }

    usleep(100000);

  }


}


void dpm_ConfigureDevice(void)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  void *configData;
  int32_t deviceId;
  int32_t result;

  DPM_BUS_DP      profibus_config;
  configData = (DPM_BUS_DP *) &profibus_config;

  deviceId    = device_getIdFromDeviceName("libpbdpm");

  // set profibusmaster busmode
  adi_GetApplicationInterface()->CallDeviceSpecificFunction("PBDPM_APP_SET_BUS_MODE", &result, ASYNCHRON_MODE);

  /* clear config buffer */
  memset(&profibus_config, 0, sizeof(profibus_config));

  profibus_config.usBus_Para_Len                 = 66;
  profibus_config.bFDL_Add                       = master_table.bFDL_Add;
  profibus_config.bBaudrate                      = master_table.bBaudrate;
  profibus_config.usTSL                          = master_table.usTSL;
  profibus_config.usMin_TSDR                     = master_table.usMin_TSDR;
  profibus_config.usMax_TSDR                     = master_table.usMax_TSDR;
  profibus_config.bTQUI                          = master_table.bTQUI;
  profibus_config.bTSET                          = master_table.bTSET;
  profibus_config.ulTTR                          = master_table.ulTTR;
  profibus_config.bG                             = master_table.bG;
  profibus_config.bHSA                           = master_table.bHSA;
  profibus_config.bMax_Retry_Limit               = master_table.bMax_Retry_Limit;
  profibus_config.Bp_Flag                        = master_table.Bp_Flag;
  profibus_config.usMin_Slave_Intervall          = master_table.usMin_Slave_Intervall;
  profibus_config.usPoll_Timeout                 = master_table.usPoll_Timeout;
  profibus_config.usData_Control_Time            = master_table.usData_Control_Time;
  profibus_config.bMasterSetting                 = master_table.bMasterSetting;
  profibus_config.bMax_User_Global_Control       = 8;
  profibus_config.usMaster_User_Data_Len         = 2;
  profibus_config.ulTCL                          = 0x8000;
  profibus_config.bMax_TSH                       = 0;


  adiResponse = adi->ConfigureDevice(deviceId, configData);

  if(adiResponse != DAL_SUCCESS)
  {
    printf("Master Configuration failed.\n");
  }
  else
  {
    dpm_CreateMasterThread();
  }

}


void dpm_CreateMasterThread(void)
{
  pthread_t dpm;
  pthread_attr_t attr;
  struct sched_param sched;
  const size_t stack_size = 0x10000;

  pthread_attr_init(&attr);
  sched.__sched_priority = 80;
  pthread_attr_setschedparam(&attr, &sched);
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize (&attr, stack_size);
  pthread_create (&dpm, &attr, dpmThread, NULL);
}

//---- End of source file ------------------------------------------------------
