/////-----------------------------------------------------------------------------
////  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
////
////  PROPRIETARY RIGHTS are involved in the subject matter of this material.
////  All manufacturing, reproduction, use and sales rights pertaining to this
////  subject matter are governed by the license agreement. The recipient of this
////  software implicitly accepts the terms of the license.
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
/////  \file     profinetdemo_idevice.c
/////
/////  \version  $Id:
/////
/////  \brief    Profinet iDevice demo program
/////
/////  \author   <Steffen Borcherding> : WAGO Kontakttechnik GmbH & Co. KG
/////
////-----------------------------------------------------------------------------
//
////-----------------------------------------------------------------------------
//// include files
////-----------------------------------------------------------------------------
#include <stddef.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "profinet/pndevice.h"
#include "profinet/processdata.h"
#include "profinet/diagnosis.h"

#define TIME_1US                          1
#define TIME_1MS                          TIME_1US * 1000
#define TIME_1SEC                         TIME_1MS * 1000

#define TASK_10MS                         TIME_1MS * 10
#define DEMO_EXECUTION_TIME               TIME_1SEC * 100

/* diagnosis example */
#define EXAMPLE_CHANNEL_ERROR_TYPE        256
#define EXAMPLE_EXT_CHANNEL_ERROR_BUS_FAULT  1



typedef struct localProcessData
{
    uint8_t unsignet8_arr[2];
    int32_t integer32;

}localProcessDataType;

static localProcessDataType out_process_data;
static localProcessDataType in_process_data;


typedef struct diagnosis
{
    bool diagnosis_in_processing;
    struct pnd_Subm* submodule_ref;
    DiagnosisHandle diagnosis_handle;
    ChannelDiagnosisSource diagmosis_source;
    ExtendedChannelDiagnosis diagnosis_extended;

}diagnosisType;

static diagnosisType example_diagnosis;


/* some functions to read and write pn-process data*/

static void ReadUnsigned8_arr(ExchangeDPList* dataPointList)
{

  ExchangeDP *datapoint = dataPointList->first_;

  if(datapoint)
  {
    uint8_t* data = (uint8_t*)((*datapoint->in_data_buffer_) + datapoint->in_data_offset);

    for(int i=0; i<sizeof(in_process_data.unsignet8_arr); i++)
    {
      in_process_data.unsignet8_arr[i] = data[i];
    }
  }

}

static void ReadInteger32(ExchangeDPList* dataPointList)
{

  ExchangeDP *datapoint = dataPointList->first_;

  if(datapoint)
  {
    int32_t* data = (int32_t*)((*datapoint->in_data_buffer_) + datapoint->in_data_offset);

    if(data != NULL)
    {
      in_process_data.integer32 = ntohl(*data);
    }
  }

}

static void WriteUnsigned8_arr(ExchangeDPList* dataPointList)
{
  ExchangeDP *datapoint = dataPointList->first_;

  if(datapoint)
  {
    /* Set Process Data */
    uint8_t* data = (uint8_t*)datapoint->out_data_;

    for(int i=0; i<sizeof(in_process_data.unsignet8_arr); i++)
    {
      data[i] = out_process_data.unsignet8_arr[i];
    }
  }

}

static void WriteInteger32(ExchangeDPList* dataPointList)
{
  ExchangeDP *datapoint = dataPointList->first_;

  if(datapoint)
  {
    /* Set Process Data */
    int32_t* data = (int32_t*)datapoint->out_data_;

    if(data != NULL)
    {
      *data = htonl(out_process_data.integer32);
    }
  }

}


/* some functions to initialize, add and remove pn-diagnosis*/

static void InitDiagnosis(diagnosisType *diagnosis){

  pnd_CfgGetSubmodule(&diagnosis->submodule_ref, 0, 1);

  diagnosis->diagmosis_source.channelNumber = 0x8000;
  diagnosis->diagmosis_source.type = unspecified;
  diagnosis->diagmosis_source.direction = Direction_ManufacturerSpecific;
  diagnosis->diagmosis_source.accumulative = 0;

  diagnosis->diagnosis_extended.severity = DiagnosisSeverity_Fault;
  diagnosis->diagnosis_extended.channelErrorType = EXAMPLE_CHANNEL_ERROR_TYPE;
  diagnosis->diagnosis_extended.extChannelErrorType = 0;
  diagnosis->diagnosis_extended.extChannelAddValue = 0;

}

static void AddDiagnosisFinishRoutine(DiagnosisHandle handle,
                                      void * user_ref, int32_t status)
{
  (void)status;
  diagnosisType *diagnosis = (diagnosisType*)user_ref;
  diagnosis->diagnosis_handle = handle;
  diagnosis->diagnosis_in_processing = false;

}

static void AddDiagnosis(diagnosisType *diagnosis)
{
  diagnosis->diagnosis_in_processing = true;
  diagnosis->diagnosis_extended.extChannelErrorType = EXAMPLE_EXT_CHANNEL_ERROR_BUS_FAULT;

  int status = pnd_DiagnosisAdd(diagnosis->submodule_ref,
                                &diagnosis->diagmosis_source,
                                &diagnosis->diagnosis_extended,
                                &AddDiagnosisFinishRoutine,
                                diagnosis);
  if(status != PN_STATUS_OK)
  {
    diagnosis->diagnosis_in_processing = false;
  }

}

static void RemoveDiagnosisFinishRoutine(DiagnosisHandle handle,
                                         void * user_ref, int32_t status)
{
  (void)status;
  (void)handle;
  diagnosisType *diagnosis = (diagnosisType*)user_ref;
  diagnosis->diagnosis_handle = NULL;
  diagnosis->diagnosis_in_processing = false;
}

static void RemoveDiagnosis(diagnosisType *diagnosis)
{
  diagnosis->diagnosis_in_processing = true;
  int status = pnd_DiagnosisRemove(diagnosis->diagnosis_handle,
                                   &RemoveDiagnosisFinishRoutine,
                                   diagnosis);
  if(status != 0)
  {
    diagnosis->diagnosis_in_processing = false;
  }
}


/* some functions to print status and process data informations*/

static void PrintApplicationStatus(uint8_t status)
{

  printf("Application status: \n");

  if(status & 0x01)
  {
    printf("  Link down \n");
  }
  else
  {
    printf("  Link up \n");
  }

  if(status & 0x02)
  {
    printf("  AR not established \n");
  }
  else
  {
    printf("  AR established \n");
  }

  if(status & 0x04)
  {
    printf("  Diagnose MaintenanceDemanded \n");
  }

  if(status & 0x08)
  {
    printf("  Diagnose Fault \n");
  }

  if(status & 0x10)
  {
    printf("  Module/Submodule difference \n");
  }

  if(status & 0x20)
  {
    printf("  Submodules Locked_by_xxx \n");
  }

  if(status & 0x80)
  {
    printf("  Device application inactive \n");
  }

}


static void PrintProcessData()
{

    printf("ProcessData \n");
    printf("  in:   unsignet8_arr[0] 0x%02x \n", in_process_data.unsignet8_arr[0]);
    printf("  in:   unsignet8_arr[1] 0x%02x \n", in_process_data.unsignet8_arr[1]);
    printf("  in:   integer32        %d   \n", in_process_data.integer32);
    printf("  out:  unsignet8_arr[0] 0x%02x \n", out_process_data.unsignet8_arr[0]);
    printf("  out:  unsignet8_arr[1] 0x%02x \n", out_process_data.unsignet8_arr[1]);
    printf("  out:  integer32        %d   \n", out_process_data.integer32);
    printf(" \n");

}


/* cyclic execution (task example) */

static void CyclicExecution(void)
{
  /* Get Data Point Lists of all Submodules */
  ExchangeDPList* dpList_Mod1_Subm1 = pnd_GetExchangeDPList(1,1);
  ExchangeDPList* dpList_Mod2_Subm1 = pnd_GetExchangeDPList(2,1);

  pnd_SetAPDUProviderState(APDU_PROVIDERSTATE_RUN);


  /* start cyclic execution */
  for(uint32_t execution_time = 0; execution_time < DEMO_EXECUTION_TIME; execution_time += TASK_10MS)
  {


    /* Update all output IOCR */
    pnd_UpdateOutIocrData();
    pnd_StartProcessDataExchange();

    /* Read Data from Module's */
    ReadUnsigned8_arr(dpList_Mod1_Subm1);
    ReadInteger32(dpList_Mod2_Subm1);

    /* Do something with input data and write output data (e.g. copy in to out and increment) */
    out_process_data.integer32 = in_process_data.integer32 +1;
    out_process_data.unsignet8_arr[0] = in_process_data.unsignet8_arr[0] +1;
    out_process_data.unsignet8_arr[1] = in_process_data.unsignet8_arr[1] +1;

    /* Write Data for all Module's */
    WriteUnsigned8_arr(dpList_Mod1_Subm1);
    WriteInteger32(dpList_Mod2_Subm1);

    /* Stop process data exchange and update all input IOCR */
    pnd_StopProcessDataExchange();
    pnd_UpdateInIocrData();




    /* example for adding and removing profinet diagnosis
     * add example diagnosis after 10 seconds
     * remove example diagnosis after 30 seconds */
    if(execution_time == TIME_1SEC * 10)
    {
      AddDiagnosis(&example_diagnosis);
      printf("Add example diagnosis \n \n");
    }
    if(execution_time == TIME_1SEC * 30)
    {
      RemoveDiagnosis(&example_diagnosis);
      printf("Remove example diagnosis \n \n");
    }



    /* print cyclic application status and process data (cyclic 1 sec)*/
    if(execution_time % (TIME_1SEC) == 0)
    {
      PrintApplicationStatus(pnd_GetApplicationStatus());
      PrintProcessData();
    }

    usleep(TASK_10MS);
  }

  pnd_SetAPDUProviderState(APDU_PROVIDERSTATE_STOP);

}


/* profinet module and submodule configuration example*/

static int32_t Configure(void)
{

  struct pnd_Iod* iod;
  struct pnd_Mod* mod;
  struct pnd_Subm* subm;

  uint8_t name[16] = "wago-idevice";
  int32_t status = PN_STATUS_FAILED;

  /* Set Station Name */
  status = pnd_CfgGetIDevice(&iod);
  pnd_CfgSetIodStationName(iod, name, 12, 0);

  /* Configure Module at Slot 0 */
  status |= pnd_CfgGetModule(&mod, 0);
  pnd_CfgSetModPlugged(mod, true);
  pnd_CfgSetModRealId(mod, 0xA0000001);

  status |= pnd_CfgGetSubmodule(&subm, 0, 1);       //device access point DAP
  pnd_CfgSetSubmPlugged(subm, true);
  pnd_CfgSetSubmRealId(subm, 0x00000001);
  status |= pnd_CfgGetSubmodule(&subm, 0, 0x8000);  //interface
  pnd_CfgSetSubmPlugged(subm, true);
  pnd_CfgSetSubmRealId(subm, 0x00001000);
  status |= pnd_CfgGetSubmodule(&subm, 0, 0x8001);  //port1 e.g. X11
  pnd_CfgSetSubmPlugged(subm, true);
  pnd_CfgSetSubmRealId(subm, 0x00001001);
  status |= pnd_CfgGetSubmodule(&subm, 0, 0x8002);  //port2 e.g. X12
  pnd_CfgSetSubmPlugged(subm, true);
  pnd_CfgSetSubmRealId(subm, 0x00001001);

  /* Configure Module at Slot 1 */
  status |= pnd_CfgGetModule(&mod, 1);
  pnd_CfgSetModPlugged(mod, true);
  pnd_CfgSetModRealId(mod, 0x00000001);

  status |= pnd_CfgGetSubmodule(&subm, 1, 1);       //byte[2] (in and out)
  pnd_CfgSetSubmPlugged(subm, true);
  pnd_CfgSetSubmRealId(subm, 0x28022802);

  /* Configure Module at Slot 2 */
  status |= pnd_CfgGetModule(&mod, 2);
  pnd_CfgSetModPlugged(mod, true);
  pnd_CfgSetModRealId(mod, 0x00000002);

  status |= pnd_CfgGetSubmodule(&subm, 2, 1);       //int (in and out)
  pnd_CfgSetSubmPlugged(subm, true);
  pnd_CfgSetSubmRealId(subm, 0x20002000);

  return status;
}


/* some event functions. (feedback of module/submodule state, configuration release finished and profinet application stopped */

static void EventDeviceStatus(uint16_t slot, uint16_t subslot, dStatus state)
{
  (void)slot;
  (void)subslot;
  (void)state;
}

static void EventConfigReleased(void)
{

}

static void EventPndAppStopped(void)
{

}

//-----------------------------------------------------------------------
///
/// main of Profinet iDevice demo program
///
//-----------------------------------------------------------------------

int
main(int argc, char *argv[])
{
  int32_t status = PN_STATUS_FAILED;

  /* Initialize Profinet */
  status = pnd_Init((eventFuncDeviceStatus)EventDeviceStatus,
                   (eventFuncConfigReleased)EventConfigReleased,
                   (eventFuncPndAppStopped)EventPndAppStopped);

  if(PN_STATUS_OK == status)
  {
    /* Start Profinet */
    status = pnd_Start();
  }

  if(PN_STATUS_OK == status)
  {
    /* Download new Profinet Configuration */
    status = Configure();

    /* example to initialize example diagnosis data*/
    InitDiagnosis(&example_diagnosis);
  }

  if(PN_STATUS_OK == status)
  {
    /* Confirm Download of new Profinet Configuration is finished */
    pnd_EventCfgAvaliable();

    /* Print new Profinet Configuration */
    pnd_CfgPrint();

    /* Example for a main Data Process*/
    CyclicExecution();
  }

  /* Stop Profinet */
  pnd_Stop();

  /* Terminate Profinet */
  pnd_Terminate();

  /* Exit Programm */
  return 0;
}

