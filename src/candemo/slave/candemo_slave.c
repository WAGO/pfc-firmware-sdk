///-----------------------------------------------------------------------------
//  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
//  PROPRIETARY RIGHTS are involved in the subject matter of this material.
//  All manufacturing, reproduction, use and sales rights pertaining to this
//  subject matter are governed by the license agreement. The recipient of this
//  software implicitly accepts the terms of the license.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///  \file     candemo_slave.c
///
///  \version  $Id:
///
///  \brief    CANopen slave demo program for ADI interface
///
///  \author   <wue> : WAGO Kontakttechnik GmbH & Co. KG
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// standard include files
//-----------------------------------------------------------------------------

#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

//-----------------------------------------------------------------------------
// include files for CANopen WAGO ADI
//-----------------------------------------------------------------------------

#include <dal/adi_application_interface.h>
#include <dal/canopen_types.h>

//-----------------------------------------------------------------------------
// defines and test setup
//-----------------------------------------------------------------------------

#define CAN_TESTPRIO 0   // no custom priority for can background loop
#define CAN_MAINPRIO 40  // priority for main loop

//-----------------------------------------------------------------------
///
/// main of CANopen slave demo program
///
/// \param none
///
//-----------------------------------------------------------------------

int
main(int argc, char *argv[])
{
  // vars for ADI-interface
  tDeviceInfo deviceList[10]; // the list of devices given by the ADI
  size_t nrDevicesFound;      // number of devices found
  size_t nrCANSlaveFound;     // position of the CANopen master in the list
  tDeviceId canDeviceId;      // device ID from the ADI
  tApplicationDeviceInterface * adi; // pointer to the application interface
  tBusState canBusState;      // var for the ADI generic bus state
  uint32_t taskId = 0;        // task Id not used in this program

  // vars for canopen interface setup
  canopen_bus_config can_slave_config; // struct for the canopen slave parameter
  canopen_cstatus can_cstate;          // compact canbus state info
  canopen_com_status can_com_state;    // communication info of the can bus
  tApplicationStateChangedEvent event; // event for ADI

  // process data
  uint8_t pd_in[4096];
  uint8_t pd_out[4096];
  uint8_t emc_msg[5];

  // generic vars
  time_t last_t = 0, new_t;
  uint32_t i;
  struct sched_param s_param;
  long unsigned runtime = 0;
  int32_t result;


  // startup info
  printf("************************************************\n");
  printf("*** DAL CANopen Slave ADI demo application V1.05\n");
  printf("************************************************\n");

  // clear process memory
  memset(pd_in, 0, sizeof(pd_in));
  memset(pd_out, 0, sizeof(pd_out));

  /* clear config buffer */
  memset(&can_slave_config, 0, sizeof(can_slave_config));

  // make config for slave
  can_slave_config.NodeId          = 2;
  can_slave_config.Baudrate        = 0;       // 4 = 125K 0 = 1M
  can_slave_config.Prio            = CAN_TESTPRIO;
  can_slave_config.DeviceType      = 0x00195;
  can_slave_config.HeartbeatPeriod = 1000;
  can_slave_config.Product_code    = 0x00151; // 750-337
  can_slave_config.Vendor_ID       = 0x00021;
  can_slave_config.Revision_number = 0x10000; // DUMMY
  can_slave_config.Serial_number   = 0; // Use electronic product label


  sprintf((char*) can_slave_config.DevName, "WAGO");      // max. 12 Bytes
  sprintf((char*) can_slave_config.HW_Ver, "HW Ver. 01"); // max. 12 Bytes
  sprintf((char*) can_slave_config.SW_Ver, "03.01.17");    // max. 8 Bytes

  // connect to ADI-Interface
  adi = adi_GetApplicationInterface();

  // Init Interface
  adi->Init();

  // scan devices
  adi->ScanDevices();
  adi->GetDeviceList(sizeof(deviceList), deviceList, &nrDevicesFound);

  // find canopen slave
  nrCANSlaveFound = -1;
  for (i = 0; i < nrDevicesFound; i++)
  {
    if (strcmp(deviceList[i].DeviceName, "libcanopens") == 0)
    {
      nrCANSlaveFound = i;
      printf("CANopen slave device found as device %i\n", i);
    }
  }

  // no canopen master found > exit
  if (nrCANSlaveFound == -1)
  {
    printf("No CANopen slave device found \n");

    // disconnect ADI-Interface
    adi->Exit();

    // exit programm
    return 0;
  }

  // open interface as slave
  canDeviceId = deviceList[nrCANSlaveFound].DeviceId;
  if (adi->OpenDevice(canDeviceId) == DAL_SUCCESS)
  {
    printf("CANopen slave device open OK\n");

    // initialize the interface and start the CANopen stack
    if (adi->ConfigureDevice(canDeviceId, &can_slave_config) == DAL_SUCCESS)
    {
      // switch to RT Priority
      s_param.sched_priority = CAN_MAINPRIO;
      sched_setscheduler(0, SCHED_FIFO, &s_param);

      // set Watchdog to 100ms
      if (adi->WatchdogSetTime(1000, 100) != DAL_SUCCESS)
         printf("Watchdog failed\n");

      // set application state running
      event.State = ApplicationState_Running;
      if (adi->ApplicationStateChanged(event) != DAL_SUCCESS)
        printf("Appication State Run failed\n");

      // start watchdog
      if (adi->WatchdogStart() != DAL_SUCCESS)
         printf("Watchdog Start failed\n");

      // run main loop for data exchange for 60s or forever
      while (1) //(runtime < 60)
      {
        // trigger watchdog
        adi->WatchdogTrigger();

        // wait 10 ms
        usleep(10000);

        // get compact bus state
        adi->DiagnoseGetDeviceState(canDeviceId, sizeof(can_cstate),
            (uint8_t*) &can_cstate, &canBusState);

        // new data from master
        if (can_cstate.Flags.NewData)
        {
          // lock and update process data input
          adi->ReadStart(canDeviceId, taskId);
          adi->ReadBytes(canDeviceId, taskId, 0,256, (uint8_t *) &pd_in[0]);
          // unlock input data
          adi->ReadEnd(canDeviceId, taskId);

          // echo process data
          memcpy(pd_out, pd_in, 256);

          // lock and write output data
          adi->WriteStart(canDeviceId, taskId);
          adi->WriteBytes(canDeviceId, taskId, 0, 256, (uint8_t *) &pd_out[0]); /* 256 byte  */

          // unlock PD-out data
          adi->WriteEnd(canDeviceId, taskId);
        }

        // 1s tick for test output
        new_t = time(NULL);
        if (new_t != last_t)
        {
          last_t = new_t;
          runtime++;

          // print info
          printf("%lu:%02lu:%02lu ", runtime / 3600ul, (runtime / 60ul) % 60ul,runtime % 60ul);

          switch (can_cstate.BusState)
            {
          case CANOPEN_STATE_INIT:
            printf("State = Init ");
          break;
          case CANOPEN_STATE_SLAVE_OP:
            printf("State = Operational ");
          break;
          case CANOPEN_STATE_SLAVE_PREOP:
            printf("State = Preoperational ");
          break;
          default:
            printf("State = %02X ", can_cstate.BusState);
          break;
            }

          printf(" Input-Data = %02X %02X %02X %02X %02X %02X %02X %02X",
              (int) pd_in[0], (int) pd_in[1], (int) pd_in[2], (int) pd_in[3],
              (int) pd_in[4], (int) pd_in[5], (int) pd_in[6], (int) pd_in[7]);

          // get extended status via ADI-device specific function
          if (adi->CallDeviceSpecificFunction("CANOPEN_STATUS", (void*) &result,
              0, (uint8_t*) &can_com_state) == DAL_SUCCESS)
          {
            // funktion call is ok
            if (result == DAL_SUCCESS)
            {
              // function result is valid
              printf(" errorinfo: %X timeouts:%i ",
                  (int) can_com_state.bus_diag,
                  (int) can_com_state.tx_timeouts);
            }
          }

          printf("\n");

          // send temperature error as EMCY
          /*
          if (runtime == 15)
            {
            emc_msg[0] = 0x12; // dummy entry
            emc_msg[1] = 0x34;
            emc_msg[2] = 0x56;
            emc_msg[3] = 0x78;
            emc_msg[4] = 0x90;


            adi->CallDeviceSpecificFunction("CANOPEN_SEND_EMC_MSG",(void*) &result,(uint32_t) 0x4000, (uint32_t) 0x29,emc_msg);
            }
           */
            
        } // 1s ..
      } // while ..

    }

    else  //  if (adi->ConfigureDevice(canDeviceId, &can_slave_config) == DAL_SUCCESS) ..

      printf("CANopen slave configure device failed\n");

    // Stop Watchdog
    if (adi->WatchdogStop() != DAL_SUCCESS)
        printf("Watchdog Stop failed\n");


    // close canopen slave
    adi->CloseDevice(canDeviceId);
    printf("CANopen slave device closed\n");
  }

  else  // if (adi->OpenDevice(canDeviceId) == DAL_SUCCESS) ..

    printf("CANopen slave device open failed\n");


  // close DAL
  adi->Exit();

  // exit programm
  return 0;

}

