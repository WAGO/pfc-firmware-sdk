///-----------------------------------------------------------------------------
//  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
//  PROPRIETARY RIGHTS are involved in the subject matter of this material.
//  All manufacturing, reproduction, use and sales rights pertaining to this
//  subject matter are governed by the license agreement. The recipient of this
//  software implicitly accepts the terms of the license.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///  \file     candemo_layer2.c
///
///  \version  $Id:
///
///  \brief    CAN layer2 demo program for ADI interface
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
// include files for CAN WAGO ADI
//-----------------------------------------------------------------------------

#include <dal/adi_application_interface.h>
#include <dal/canopen_types.h>

//-----------------------------------------------------------------------------
// defines and test setup
//-----------------------------------------------------------------------------

#define CAN_TESTPRIO 0   // no custom priority for can background loop
#define CAN_MAINPRIO 20  // priority for main loop
#define CAN_MAX_IDS  16  // maximum 11 bit ids for echo
#define CAN_MAX_IDS2 16  // maximum 29 bit ids for echo
//#define CAN_ECHO         // echo layer data for ibw endurance test
#define CAN_RXBASE  0x6A0
#define CAN_RXBASE2 0x80000000 + 0x6C0


//-----------------------------------------------------------------------
///
/// main of CANopen slave demo program
///
/// \param none
///
//-----------------------------------------------------------------------

int main(int argc, char *argv[])
{
  // vars for ADI-interface
  tDeviceInfo                   deviceList[10];   // the list of devices given by the ADI
  size_t                        nr_devices_found; // number of devices found
  int32_t                       nr_canlayer2;     // position of the CAN Layer2 Device in the list
  tDeviceId                     can_device_id;    // device ID from the ADI
  tApplicationDeviceInterface * adi;              // pointer to the application interface
  tApplicationStateChangedEvent event;            // event for ADI
  int32_t                       result;           // return value

  // vars for can interface
  canopen_com_status            can_com_state;    // communication info of the can bus
  uint32_t                      can_baudrate = 1000000;;
  uint8_t                       can_data[8];
  int32_t                       can_dlc;
  uint32_t                      can_id;

  // generic vars
  time_t               last_t = 0;
  time_t               new_t;
  struct sched_param   s_param;
  long unsigned        uptime = 0;
  long unsigned        runtime = 120;
  long unsigned        cantime = 0;
  unsigned        	   delay   = 200;
  int                  i;
  long unsigned        messages = 0;

  // startup info
  printf("************************************************\n");
  printf("*** DAL CAN Layer2 ADI demo application V0.11   \n");
  printf("*** use: candemo_layer2 runtime baudrate delay  \n");
  printf("*** runtime = 0 forever default = 120s          \n");
  printf("*** baudrate default = 1MBit                    \n");
  printf("*** delay default = 200us                       \n");
  printf("************************************************\n");

  // scan command line options
  if (argc > 1)
  {
	runtime = (unsigned long) atol(argv[1]);
    if ((runtime < 10) && (runtime > 0))
	  runtime = 10;

    if (argc > 2)
    {
      can_baudrate = (unsigned long) atol(argv[2]);
      if (can_baudrate < 20000)
    	  can_baudrate = 20000;
    }

    if (argc > 3)
    {
      delay = (unsigned) atoi(argv[3]);
      if (delay < 200)
      	  delay = 200;
    }
  }

  // print setting
  if (runtime == 0)
    printf("unlimited runtime\r\n");
  else
    printf("runtime = %is\r\n",(int) runtime);
  printf("baudrate = %li\r\n",(long) can_baudrate);
  printf("delay between loops %i\r\n",delay);

  // connect to ADI-Interface
  adi = adi_GetApplicationInterface();

  // init Interface
  adi->Init();

  // scan devices
  adi->ScanDevices();
  adi->GetDeviceList(sizeof(deviceList), deviceList, &nr_devices_found);

  // find canopen slave
  nr_canlayer2 = -1;
  for (i = 0; i < (int32_t) nr_devices_found; i++)
  {
    if (strcmp(deviceList[i].DeviceName, "libcanlayer2") == 0)
    {
      nr_canlayer2 = i;
      printf("CAN layer2 device found as device %i\n", i);
    }
  }

  // no canopen layer2 device found > exit
  if (nr_canlayer2 == -1)
  {
    printf("No CAN layer2 device found \n");

    // disconnect ADI-Interface
    adi->Exit();

    // exit programm
    return 0;
  }

  // open interface as layer2 device
  can_device_id = deviceList[nr_canlayer2].DeviceId;
  if (adi->OpenDevice(can_device_id) == DAL_SUCCESS)
  {
    printf("CANopen layer2 device open OK\n");

    if (adi->ConfigureDevice(can_device_id, NULL) == DAL_SUCCESS)
      printf("CANopen layer2 configure device OK\n");

    // switch to RT priority
    s_param.sched_priority = CAN_MAINPRIO;
    sched_setscheduler(0, SCHED_FIFO, &s_param);

    // set watchdog to 100ms
    if (adi->WatchdogSetTime(1000, 100) != DAL_SUCCESS)
       printf("Watchdog failed\n");

    // set application state running
    event.State = ApplicationState_Running;
    if (adi->ApplicationStateChanged(event) != DAL_SUCCESS)
      printf("Appication State Run failed\n");

    // start watchdog
    if (adi->WatchdogStart() != DAL_SUCCESS)
         printf("Watchdog Start failed\n");

    // open CAN interface
    if (adi->CallDeviceSpecificFunction("CANOPEN_L2OPEN", (void*) &result, can_baudrate) == DAL_SUCCESS)
    {
      for (can_id = CAN_RXBASE ; can_id < (CAN_RXBASE + CAN_MAX_IDS) ; can_id++)
        if (adi->CallDeviceSpecificFunction("CANOPEN_REGISTER_ID", (void*) &result, (uint32_t) 1, can_id) != DAL_SUCCESS)
           printf("register id failed\n");
      for (can_id = CAN_RXBASE2 ; can_id < (CAN_RXBASE2 + CAN_MAX_IDS2) ; can_id++)
        if (adi->CallDeviceSpecificFunction("CANOPEN_REGISTER_ID", (void*) &result, (uint32_t) 1, can_id) != DAL_SUCCESS)
           printf("register id failed\n");

      // loop
      while ( ((runtime > 0) && (runtime > uptime)) || (runtime == 0))
      {
        // trigger watchdog
        adi->WatchdogTrigger();

        // wait for other tasks
        usleep(delay);

        for (can_id = CAN_RXBASE ; can_id < (CAN_RXBASE + CAN_MAX_IDS) ; can_id++)
        {
          adi->CallDeviceSpecificFunction("CANOPEN_GETFRAMECOUNT",(void*)&result,(uint32_t)can_id);
          if (result > 0)
          {
            adi->CallDeviceSpecificFunction("CANOPEN_GETFRAME",(void*)&can_dlc,&can_id,can_data,0);
            //printf("Message for ID %x DLC %i \n",can_id,can_dlc);
            if (can_dlc > 0)
            {
              if (adi->CallDeviceSpecificFunction("CANOPEN_SENDFRAME",(void*)&result,(uint32_t)can_id + 0x10 ,can_data,can_dlc,1000)
                != DAL_SUCCESS)
                  printf("Send Message fail for ID %x\n",can_id + 0x80 );
              messages++;
            }
          }
        }

        for (can_id = CAN_RXBASE2 ; can_id < (CAN_RXBASE2 + CAN_MAX_IDS2) ; can_id++)
        {
          adi->CallDeviceSpecificFunction("CANOPEN_GETFRAMECOUNT",(void*)&result,(uint32_t)can_id);
          if (result > 0)
          {
            adi->CallDeviceSpecificFunction("CANOPEN_GETFRAME",(void*)&can_dlc,&can_id,can_data,0);
            //printf("Message for ID %x DLC %i \n",can_id,can_dlc);
            if (can_dlc > 0)
            {
              if (adi->CallDeviceSpecificFunction("CANOPEN_SENDFRAME",(void*)&result,(uint32_t)can_id + 0x10 ,can_data,can_dlc,1000)
                != DAL_SUCCESS)
                  printf("Send Message fail for ID %x\n",can_id + 0x80 );
              messages++;
            }
          }
        }

        // 1s tick for test output
        new_t = time(NULL);
        if (new_t != last_t)
        {
          last_t = new_t;
          uptime++;

          if (messages > 0)
            cantime++;

          // print info
          printf("up %lu:%02lu:%02lu ", uptime / 3600ul, (uptime / 60ul) % 60ul,uptime % 60ul);
          printf("can %lu:%02lu:%02lu ", cantime / 3600ul, (cantime / 60ul) % 60ul,cantime % 60ul);

          // get extended status via ADI-device specific function
          if (adi->CallDeviceSpecificFunction("CANOPEN_STATUS", (void*) &result,
              0, (uint8_t*) &can_com_state) == DAL_SUCCESS)
          {
            // funktion call is ok
            if (result == DAL_SUCCESS)
            {
              // function result is valid
              printf(" errorinfo: %X tx overflows: %i messages/s: %li",
                  (int) can_com_state.bus_diag,
                  (int) can_com_state.tx_l2_overflows,messages);
            }
            else
              printf("CANOPEN_STATUS fail\n");
          }
          printf("\n");
          messages = 0;
        } // 1s ..
      } // while ..

      // stop watchdog
       if (adi->WatchdogStop() != DAL_SUCCESS)
           printf("Watchdog Stop failed\n");

      // close CAN interface
      adi->CallDeviceSpecificFunction("CANOPEN_L2CLOSE", (void*) &result);

    } // if if (adi->CallDeviceSpecificFunction("CANOPEN_L2OPEN"
    else
      printf("CAN layer2 open failed \n");

    // close can layer2 device
    adi->CloseDevice(can_device_id);
    printf("CAN layer2 device closed\n");
  }

  else  // if (adi->OpenDevice(can_device_id) == DAL_SUCCESS) ..

    printf("CAN layer2 device open failed\n");

  // close DAL
  adi->Exit();

  // exit programm
  return 0;

}

