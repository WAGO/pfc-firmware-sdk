//-----------------------------------------------------------------------------
//  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
//  PROPRIETARY RIGHTS are involved in the subject matter of this material.
//  All manufacturing, reproduction, use and sales rights pertaining to this
//  subject matter are governed by the license agreement. The recipient of this
//  software implicitly accepts the terms of the license.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///  \file     candemo_master.c
///
///  \version  $Id:
///
///  \brief    CANopen master demo program for ADI interface
///
///  \author   <wue> : WAGO Kontakttechnik GmbH & Co. KG
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

//-----------------------------------------------------------------------------
// include files for CANopen WAGO ADI
//-----------------------------------------------------------------------------

#include <dal/adi_application_interface.h>
#include <dal/canopen_types.h>

//-----------------------------------------------------------------------------
// defines and test setup
//-----------------------------------------------------------------------------

// mandantory slave 750-337 on node NodeID 3 at 1MBit with Modules 430,530,600
// optional   slave PFC200 with candemo_slave programm running

//#define CAN_SLAVE_COUNT 1   // 1 slave 750-337 ID 3
#define CAN_SLAVE_COUNT 2   // + slave PFC200 ID 2

// list length for slave configuration
#define CAN_SLAVE_MAX_SDOS 16

// priorities
#define CAN_TESTPRIO 0        // can background loop 0 = default
#define CAN_MAINPRIO 40       // main loop
// runtime of the test
#define CAN_TESTTIME 60*60*24

#define LOOPTIME 10     // speed for data exchange

// print status on console
//#define PRINT_INFO

// test adi function
//#define TEST_ADISTATE
//#define TEST_ADINODESTATE
//#define TEST_ADIFUNCTION
//#define TEST_ADIFUNCTION_SDO // perform sdo performance test in sync mode (blocking until answer or timeout )
//#define CAN_SDO_ASYNC   // perform sdo performance test in async mode (non blocking)

// #define TEST_MAXTIME  // perform delay test

//-----------------------------------------------------------------------------
// global structures
//-----------------------------------------------------------------------------

// our slave SDO list for the test / just the minimal config
canopen_sdo_table can_slave_sdo_list[CAN_SLAVE_COUNT][CAN_SLAVE_MAX_SDOS] =
  {
  // uint16_t index, uint8_t sub_index, uint16_t len, uint32_t value

  // config of slave 1 node ID 3 : 750-337 default values
  {
    // index 1005H of the CANopen slave: COB ID of SYNC message
    { 0x1005, 0, 4, 0x80000080 },

    // index 1017H of the CANopen slave: heartbeat producer time */
    { 0x1017, 0, 2, 555 },

  }


#if (CAN_SLAVE_COUNT == 2)

  // config of slave 2 PFC node ID 2
  ,{
    // index 1005H of the CANopen slave: COB ID of SYNC message
    { 0x1005, 0, 4, 0x80000080 },

    // index 1017H of the CANopen slave: heartbeat producer time
    { 0x1017, 0, 2, 888},

    // configure slave RPDO
    { 0x1400, 1, 4, 0x80000000 }, // disable RPDO
	{ 0x1600, 0, 1, 0 },          // disable Mapping
    { 0x1600, 1, 4, 0xA8C00140 }, // unsigned64 process data var1
    { 0x1600, 0, 1, 0x01 },       // 1 entry
    { 0x1400, 1, 4, 0x00000202 }, // enable RPDO on ID 202

    // configure Slave TPDO
    { 0x1800, 1, 4, 0x80000000 }, // disable TPDO
    { 0x1A00, 0, 1, 0 },          // disable mapping
    { 0x1A00, 1, 4, 0xA4400140 }, // unsigned64 Process Data Var1
    { 0x1A00, 0, 1, 1 },          // 1 entry

    { 0x1800, 2, 1, 0xFF },       // transmission asynchronous
    { 0x1800, 3, 2, 0 },          // inhibit time 0
    { 0x1800, 5, 2, 0 },          // event timer 0
    { 0x1800, 1, 4, 0x000000182 } // enable TPDO on ID 182
  },

#endif
};

// our master PDO configuration
canopen_pdo can_slave_pdo_list[CAN_SLAVE_COUNT][2] =
{
  // entrys in data array:
  // 1. type RPDO = 1 TPDO = 2
  // 2. length of the PDO in Bytes
  // 3. cob_id of the PDO
  // 4. trans_type 0xFF = async
  // 5. inhibit_time 0 = no inhibit time
  // 6. event_time 0 = no PDO sending on timer
  // 7. unsigned char channel_count;
  // 8. canopen_pdo_channel  * channel;

  // config for slave 1 node ID 3 : 750-337
  {
    // 1. slave RPDO for module 530
    { 1, 1, 0x203, 0xFF, 0, 0, 0, 0},
    // 1. slave TPDO for module 430
    { 2, 1, 0x183, 0xFF, 0, 0, 0, 0}
  },

#if (CAN_SLAVE_COUNT == 2)

  // config for slave 2 PFC200 node ID 2
  {
    // 2. slave RPDO
    { 1, 8, 0x202, 0xFF, 0, 0, 0, 0}, // 8 Byte process data receive
    // 2. slave TPDO
    { 2, 8, 0x182, 0xFF, 0, 0, 0, 0}, // 8 Byte process data send
  },

#endif



};

//-----------------------------------------------------------------------
///
/// main of CANopen master demo program
///
/// \param none
///
//-----------------------------------------------------------------------

int main(void)
{
  // vars for ADI-interface
  tDeviceInfo deviceList[10];          // the list of devices given by the ADI
  size_t nrDevicesFound;               // number of devices found
  size_t nrCANMasterFound;             // position of the CANopen master in the list
  tDeviceId canDeviceId;               // device ID from the ADI
  tApplicationDeviceInterface * adi;   // pointer to the application interface
  tBusState canBusState;               // var for the ADI generic bus state
  uint32_t taskId = 0;                 // task Id not used in this program
  tApplicationStateChangedEvent event; // var for the event interface of the ADI
  tDiagnoseState canDiagState;         // var for the ADI generic diagnostic state

  // vars for canopen interface
  canopen_bus_config can_master_config;                 // struct for the canopen master parameter
  canopen_node_config can_node_config[CAN_SLAVE_COUNT]; // struct for the node parameter
  canopen_sdo * can_node_sdos;                          // pointer to the nodes config for the master
  canopen_cstatus can_cstate;                           // compact canbus state info
  canopen_com_status can_com_state;                     // communication info of the can bus

  uint32_t slave;              // loop counter for setup
  uint32_t sdo_size_slave;     // size of the sdo config of the slave
  uint8_t * sdo_data_p = NULL; // pointer to the next sdo
  uint16_t can_slave_flags;    // bitlist for subdevice state

  // process data
  uint8_t pd_in[4096];    // slave to master
  uint8_t pd_out[4096];   // master to slave

  // generic vars
  int i = 0, loops = 0, t = 0;
  time_t last_t = 0, new_t;
  long unsigned runtime = 0;
  struct sched_param s_param;
  int32_t result;
  int16_t param16;

  struct timeval  timerval1;
  struct timespec timerval2;
  int32_t timer_run1;
  int32_t timer_run2;
  int32_t timer_run3;
  int32_t timerval_old1 = 0;
  int32_t timerval_old2 = 0;
  int32_t timerval_old3 = 0;
  int32_t timer_max1 = 0;
  int32_t timer_max2 = 0;
  int32_t timer_max3 = 0;

  // startup info */
  printf("**************************************************\n");
  printf("*** DAL CANopen Master ADI Demo Application V1.13 \n");
  printf("**************************************************\n");

  // clear process memory
  memset(pd_in, 0, sizeof(pd_in));
  memset(pd_out, 0, sizeof(pd_out));

  // connect to ADI-interface
  adi = adi_GetApplicationInterface();

  // init interface
  adi->Init();

  // scan devices
  adi->ScanDevices();
  adi->GetDeviceList(sizeof(deviceList), deviceList, &nrDevicesFound);

  // find canopen master
  nrCANMasterFound = -1;
  for (i = 0; i < nrDevicesFound; ++i)
  {
    if (strcmp(deviceList[i].DeviceName, "libcanopenm") == 0)
    {
      nrCANMasterFound = i;
      printf("CANopen master device found as device %i\n", i);
    }
  }

  // no canopen master found > exit
  if (nrCANMasterFound == -1)
  {
    printf("No CANopen master device found \n");

    // disconnect ADI-Interface
    adi->Exit();

    // exit programm
    return 0;
  }

  // open canopen master
  canDeviceId = deviceList[nrCANMasterFound].DeviceId;
  if (adi->OpenDevice(canDeviceId) == DAL_SUCCESS)
  {
    printf("CANopen master device open OK\n");

    // clear master config buffer
    memset(&can_master_config, 0, sizeof(can_master_config));

    // make config for master
    can_master_config.NodeId                = 1;
    can_master_config.NumOfNodes            = CAN_SLAVE_COUNT;
    can_master_config.Baudrate              = 0;       // 4 = 125K 0 = 1M
    can_master_config.HeartbeatPeriod       = 500;
    can_master_config.Prio                  = CAN_TESTPRIO;
    can_master_config.DeviceType            = 0x00195; // DS 405 no IO
    can_master_config.Product_code          = 1000;    // DUMMY
    can_master_config.Vendor_ID             = 0x00021;
    can_master_config.SyncCobId             = 0x00000080;
    can_master_config.CommCyclePeriod       = 0x0;
    can_master_config.TimeStampId           = 0x100UL; // consumer
    can_master_config.EmergencyId           = 0;
    can_master_config.GuardTime             = 0;
    can_master_config.LifeTime              = 0;
    can_master_config.Revision_number       = 0x10000; // DUMMY
    can_master_config.Serial_number         = 0; // Use electronic product label
    can_master_config.ConfigState.AutoStart = 1; // start the master and the nodes

    sprintf((char*) can_master_config.DevName,"WAGO");       // max. 12 Bytes
    sprintf((char*) can_master_config.HW_Ver, "HW Ver. 01"); // max. 12 Bytes
    sprintf((char*) can_master_config.SW_Ver, "03.01.17");    // max. 8 Bytes

    // clear node config buffer
    memset(can_node_config, 0, sizeof(can_node_config));

    // config for slave 1 759-337
    can_node_config[0].tab.NodeId = 3;
    can_node_config[0].tab.PDOCount = 2;
    can_node_config[0].tab.SDOCount = 2;
    can_node_config[0].tab.ConfigState.DisableStartNode = 0;
    can_node_config[0].tab.ConfigState.DisableCANopenStartup = 0;
    can_node_config[0].tab.ConfigState.Mandantory = 0;

 
    // config for slave 2 PFC200
   #if (CAN_SLAVE_COUNT == 2)

    can_node_config[1].tab.NodeId = 2;
    can_node_config[1].tab.PDOCount = 2;
    can_node_config[1].tab.SDOCount = 15;
  
  #endif

    // initialize the interface and start the CANopen master manager
    if (adi->ConfigureDevice(canDeviceId, &can_master_config) == DAL_SUCCESS)
    {
      // prepare slave node config for master
      for (slave = 0; slave < can_master_config.NumOfNodes; slave++)
      {
        printf("CANopen master configure subdevice %i ", (int) slave + 1);

        // point slave pdo config to master subdevice configuration
        can_node_config[slave].pdo_cfg.len  = can_node_config[slave].tab.PDOCount * sizeof(canopen_pdo);
        can_node_config[slave].pdo_cfg.pdos = &can_slave_pdo_list[slave][0];

        // scan slave node sdos for size
        sdo_size_slave = 0;
        for (i = 0; i < can_node_config[slave].tab.SDOCount; i++)
        {
          sdo_size_slave += 2 + 1 + 2; /* Index + Sub + Size */
          sdo_size_slave += can_slave_sdo_list[slave][i].len;
        }

        // allocate memory for SDO config
        can_node_sdos = malloc(sdo_size_slave);
        if (can_node_sdos == NULL)
        {
          printf("no memory! \n");
          return (-1);
        }

        // copy SDOs from list to masters slave configuration
        can_node_config[slave].sdo_cfg.sdos = can_node_sdos;
        can_node_config[slave].sdo_cfg.len = sdo_size_slave;
        sdo_data_p = (uint8_t *) can_node_sdos;
        for (i = 0; i < can_node_config[slave].tab.SDOCount; i++)
        {
          /* index,sub,len,value */
          memcpy(sdo_data_p, &can_slave_sdo_list[slave][i].index, 2);
          sdo_data_p += 2;
          memcpy(sdo_data_p, &can_slave_sdo_list[slave][i].sub_index, 1);
          sdo_data_p += 1;
          memcpy(sdo_data_p, &can_slave_sdo_list[slave][i].len, 2);
          sdo_data_p += 2;
          memcpy(sdo_data_p, &can_slave_sdo_list[slave][i].value,
              can_slave_sdo_list[slave][i].len);
          sdo_data_p += can_slave_sdo_list[slave][i].len;
        }

        // send slave config to master
        if (adi->ConfigureSubdevice(canDeviceId, slave, &can_node_config[slave])
            != DAL_SUCCESS)
          printf("failed\n");
        else
          printf("OK\n");

        // free config memory
        free(can_node_sdos);
      } // for (slave = 0; slave < can_master_config.NumOfNodes; slave++)

      printf("CANopen master configure %i subdevices done \n",
          (int) can_master_config.NumOfNodes);

      // switch to RT Priority
      s_param.sched_priority = CAN_MAINPRIO;
      sched_setscheduler(0, SCHED_FIFO, &s_param);

      // set watchdog to 100ms
      if (adi->WatchdogSetTime(1000, 100) != DAL_SUCCESS)
          printf("Watchdog failed\n");

      // Start Watchdog
      if (adi->WatchdogStart() != DAL_SUCCESS)
          printf("Watchdog Start failed\n");

      // set application state running to set master to operational
      event.State = ApplicationState_Running;
      if (adi->ApplicationStateChanged(event) != DAL_SUCCESS)
        printf("Appication State Run failed\n");

      // run main loop for CAN_TESTTIME in s
      while (runtime < CAN_TESTTIME)
      {
        usleep(1000); // wait 1 ms
        loops++;

        // trigger watchdog
        adi->WatchdogTrigger();

        // get compact bus state
       #ifdef TEST_ADISTATE
        adi->DiagnoseGetDeviceState(canDeviceId, sizeof(can_cstate),
            (uint8_t*) &can_cstate, &canBusState);
       #endif

        // 1s tick for test output
        new_t = time(NULL);
        if (new_t != last_t)
        {
          last_t = new_t;
          runtime++;

         #ifdef PRINT_INFO
          // print info
          printf("%lu:%02lu:%02lu ",runtime/3600ul,(runtime/60ul)%60ul,runtime%60ul);

         #ifdef TEST_ADISTATE
          printf("Master state = ");

          switch (can_cstate.BusState)
          {
            case CANOPEN_STATE_PREPARE_NET_INIT :
            printf("Prepare net init ");
            break;
            case CANOPEN_STATE_NTW_RESET :
            printf("Reset communication all nodes ");
            break;
            case CANOPEN_STATE_NTW_WAIT :
            printf("Wait state    ");
            break;
            case CANOPEN_STATE_BOOT_CONF :
            printf("Bootup configuration ");
            break;
            case CANOPEN_STATE_SLAVE_PREOP :
            printf("Slave preoperational ");
            break;
            case CANOPEN_STATE_RUN :
            printf("Run ");
            break;
            case CANOPEN_STATE_INIT :
            printf("Init ");
            break;
            case CANOPEN_STATE_MASTER_STATE_RESET :
            printf("Master reset ");
            break;
            case CANOPEN_STATE_PREOPERATIONAL :
            printf("Master preoperational ");
            break;
            default :
            printf("%02X ",can_cstate.BusState);
            break;
          }

          // test can status flags
          if (can_cstate.Flags.BusOff)
          printf("Bus off  ");

          if (can_cstate.Flags.BusWarn)
          printf("Bus error  ");

         #endif //  #ifdef TEST_ADISTATE

          // test 750-337 slave 1 status

         #ifdef TEST_ADINODESTATE
          adi->DiagnoseGetSubdeviceState(canDeviceId,3,sizeof(can_slave_flags),(uint8_t*) &can_slave_flags,&canDiagState);

          if (can_slave_flags & CANOPEN_SLAVE_FLAG_OPER)
          printf(" Slave state = operational ");
          if (can_slave_flags & CANOPEN_SLAVE_FLAG_PREOP)
          printf(" Slave state = preoperational ");
          if (can_slave_flags & CANOPEN_SLAVE_FLAG_STOP)
          printf(" Slave state = stop ");
         #endif

          // show loops/s
          printf("\n        Loop/s = %i ",loops);
          loops = 0;

          // show process data
          if (can_cstate.BusState == CANOPEN_STATE_RUN)
          printf(" Input Data = %02X Output data = %02X ",(int) pd_in[0],(int) pd_out[0]);

          // get extended status via ADI-device specific function
         #ifdef TEST_ADIFUNCTION
          if (adi->CallDeviceSpecificFunction("CANOPEN_STATUS",(void*)&result, 0,(uint8_t*) &can_com_state) == DAL_SUCCESS)
          {
            // funktion call is ok
            if (result == DAL_SUCCESS)// function result is valid
            printf(" errorinfo: %X timeouts:%i ",(int)can_com_state.bus_diag,(int)can_com_state.msg_timeouts);
          }
          printf("\n"); 
         #endif

		 #else
          // print minimal info
          //printf("%lu:%02lu:%02lu ",runtime/3600ul,(runtime/60ul)%60ul,runtime%60ul);
          // show loops/s
          //printf(" Loop/s = %i\n",loops);
          //printf("\nPLC Time Max = %i ",timer_max1);
          //printf("CAN Time Max = %i\n",timer_max2);
          loops = 0;
         #endif // #ifdef PRINT_INFO

          // test end > slaves to preoperational
          if (runtime == CAN_TESTTIME - 2)
          {
        	printf("\nPLC Time Max = %i ",timer_max1);
        	printf("CAN Time 337 Max = %i\n",timer_max2);
        	printf("CAN Time PFC Max = %i\n",timer_max3);

            // send goto preoperational to all slaves via ADI-device specific function
            adi->CallDeviceSpecificFunction("CANOPEN_CHANGE_STATE",
                (void*) &result, (uint32_t) 0, (uint32_t) 0x7F);
          }
        }

        /* check for timer increment */
        /* gettimeofday(&timerval, NULL); >> WAT 25204 */
        clock_gettime(CLOCK_MONOTONIC,&timerval2);
        timerval1.tv_sec  =  timerval2.tv_sec;
        timerval1.tv_usec =  timerval2.tv_nsec / 1000;

        if (timerval1.tv_usec < timerval_old1)
            timerval_old1 -= 1000000;

        if (timerval1.tv_usec < timerval_old2)
            timerval_old2 -= 1000000;

        if (timerval1.tv_usec < timerval_old3)
            timerval_old3 -= 1000000;

        timer_run1 = (timerval1.tv_usec - timerval_old1) / 1000;
        timer_run2 = (timerval1.tv_usec - timerval_old2) / 1000;
        timer_run3 = (timerval1.tv_usec - timerval_old3) / 1000;
        //timerval_old1 = timerval1.tv_sec;
        timerval_old1 = timerval1.tv_usec;

        if ((runtime > 15) && (runtime < CAN_TESTTIME - 2))
        {
          if (timer_run1 > timer_max1)
          {
            timer_max1 = timer_run1;
            //printf("PLC Timer Max = %i\n",timer_max1);
          }
          if (timer_run2 > timer_max2)
          {
            timer_max2 = timer_run2;
            //printf("CAN Timer 337 Max = %i\n",timer_max2);
          }
          if (timer_run3 > timer_max3)
          {
            timer_max3 = timer_run3;
            //printf("CAN Timer PFC Max = %i\n",timer_max3);
          }
        }
        else
        {
          timer_max1 = 1;
          timer_max2 = 1;
          timer_max3 = 1;
         }

        // test for new data from slaves
       #ifdef TEST_ADISTATE
        if (can_cstate.Flags.NewData)
        {
          // update process data input
          adi->ReadStart(canDeviceId, taskId);
          adi->ReadBytes(canDeviceId, taskId, 0, 1, (uint8_t *) &pd_in[0]);
          /* unlock PD-In data */
          adi->ReadEnd(canDeviceId, taskId);
        }

        // master in operational state > data exchange
        if (can_cstate.BusState == CANOPEN_STATE_RUN)
       #endif
        {
          // speed limit for new data
          if (t++ >= LOOPTIME)
          {
            t = 1;
            if (i < 1000)
              i++;

	         #ifdef TEST_ADIFUNCTION_SDO

            if (i == 1000) // get heartbeat register from slave
            {

             #ifdef CAN_SDO_ASYNC
              // node ID,index,subindex,datalen,pdata,timeout,SDO-channel
              if (adi->CallDeviceSpecificFunction("CANOPEN_SDO_READ_ASYNC",(void*)&result,3,0x1017,0,2,(uint8_t *) &param16,0,1) == DAL_SUCCESS)
              {

                // funktion call is ok
                if ((result != CANOPEN_SDO_WORKING) && (result != CANOPEN_SDO_STARTED)) /* only for async mode */
                {

             #else
              // node ID,index,subindex,datalen,pdata,timeout
              if (adi->CallDeviceSpecificFunction("CANOPEN_SDO_READ",(void*)&result,3,0x1017,0,2,(uint8_t *) &param16,0) == DAL_SUCCESS)
                {
                // funktion call is ok
             #endif

                  if (result >= CANOPEN_ERR_SDO)
                  {
                    printf("SDO Read Error ");
                    switch(result)
                    {
                      case CANOPEN_ERR_SDO + CANOPEN_ERR_DATATYPE_LENGTH_TO_LOW:
                        printf("DATATYPE_LENGTH_TO_LOW\n");
                        break;
                      case CANOPEN_ERR_SDO +  CANOPEN_ERR_TIMEOUT:
                        printf("SDO TIMEOUT\n");
                        break;
                      default:
                        printf("SDO ABORT Code %08lX\n",(unsigned long)  result);
                    }
                  }
                  else // no error
                  {
                    if (result != 2)
                      printf("SDO Read: DATATYPE_LENGTH != 2 (%i)\n",(int)result );
                    else
                      printf("\nHeartbeat Slave Id 3 %i ms\n",(int)param16);
                  }

                  i = 1001; // next SDO Task

                 #ifdef CAN_SDO_ASYNC
	              }  // if ((result != CANOPEN_SDO_WORKING) && (result != CANOPEN_SDO_STARTED)) /* only for async mode */
                 #endif
              }
              else
                {
                printf("SDO Read ADI Error\n");
                i = 1001; // next SDO Task
                }
            } // if (i == 1000)


            if (i == 1001) // write heartbeat register to slave
            {
              param16 += 10;

             #ifdef CAN_SDO_ASYNC

              // node ID,index,subindex,datalen,pdata,timeout,SDO-channel
              if (adi->CallDeviceSpecificFunction("CANOPEN_SDO_WRITE_ASYNC",(void*)&result,3,0x1017,0,2,(uint8_t *) &param16,0,2) == DAL_SUCCESS)
              {

                // funktion call is o
                  memset(&pd_out[2], (i >> 8) & 0xFF, 8);k
                if ((result != CANOPEN_SDO_WORKING) && (result != CANOPEN_SDO_STARTED)) // only for async mode
                {
             #else
              // Node ID,Index,Subindex,Datalen,Pdata,Timeout
              if (adi->CallDeviceSpecificFunction("CANOPEN_SDO_WRITE",(void*)&result,3,0x1017,0,2,(uint8_t *) &param16,0) == DAL_SUCCESS)
              {
                // funktion call is ok
             #endif
                  if (result >= CANOPEN_ERR_SDO)
                  {
                    printf("SDO Write Error ");pd_out[4] = (uint8_t) (timer_max2 & 0xFF);
                    switch(result)
                    {
                     case CANOPEN_ERR_SDO +  CANOPEN_ERR_TIMEOUT:
                         printf("SDO TIMEOUT\n");
                         break;
                      default:
                        printf("SDO ABORT Code %08lX\n",(unsigned long) result);
                  
                    }
                  }
                  else // no error
                    printf("Heartbeat Id 3 Write OK \n");

                  i = 0;// new SDO timer

               #ifdef CAN_SDO_ASYNC
                } // if ((result != CANOPEN_SDO_WORKING) && (result != CANOPEN_SDO_STARTED))
              #endif
              }
              else
                {
                printf(" SDO Write ADI Error\n");

                i = 0; // new SDO Timer
                }

            } // if (i == 1001)

          #else
            if (i >= 1000)
              i = 0;
          #endif

            // update process data input
            adi->ReadStart(canDeviceId, taskId);
		   #if (CAN_SLAVE_COUNT == 1)
            adi->ReadBytes(canDeviceId, taskId, 0, 1, (uint8_t *) &pd_in[0]);
           #else
            adi->ReadBytes(canDeviceId, taskId, 0, 9, (uint8_t *) &pd_in[0]);
           #endif
            /* unlock PD-In data */
            adi->ReadEnd(canDeviceId, taskId);


            // 750-337 Data  ID 3
            if (pd_in[0] == pd_out[0]) // Data test
            {
               pd_out[0] =  (uint8_t) (pd_out[0] + 1);
               timerval_old2 = timerval1.tv_usec;
            }

           #if (CAN_SLAVE_COUNT == 2)
            if ((pd_in[1] == pd_out[1]) && (pd_in[2] == pd_out[2])) // Data test
            { // PFC200 Data
              pd_out[1] = (uint8_t) (pd_in[1] + 1);
              pd_out[2] = (uint8_t) (pd_in[2] + 2);
              pd_out[3] = (uint8_t) (timer_max1 & 0xFF);
              pd_out[4] = (uint8_t) (timer_max2 & 0xFF);
              pd_out[5] = (uint8_t) (timer_max3 & 0xFF);
              timerval_old3 = timerval1.tv_usec;
            }
           #endif

            // lock an write PD-out data
            adi->WriteStart(canDeviceId, taskId);
            adi->WriteBytes(canDeviceId,taskId,0,1,(uint8_t *) &pd_out[0]); // 750-337 ID 3

            #if (CAN_SLAVE_COUNT == 2)
             adi->WriteBytes(canDeviceId, taskId, 1, 8, (uint8_t *) &pd_out[1]); // PFC200 ID 2
            #endif

            // unlock PD-Data
            adi->WriteEnd(canDeviceId, taskId);
          } // t1+

        } // can_cstate.BusState == CANOPEN_STATE_RUN

      } // while ..

    }

    else
      // if (adi->ConfigureDevice(canDeviceId,&can_master_config) == DAL_SUCCESS)

      printf("CANopen master configure device failed\n");

    // stop Watchdog
    if (adi->WatchdogStop() != DAL_SUCCESS)
        printf("Watchdog Stop failed\n");

    // stop bus
    event.State = ApplicationState_Unconfigured;
    adi->ApplicationStateChanged(event);

    // close canopen master
    adi->CloseDevice(canDeviceId);
  }

  else // if (adi->OpenDevice(canDeviceId) == DAL_SUCCESS) ...

    printf("CANopen master device open failed\n");

  // close DAL
  adi->Exit();

  // exit program
  return 0;

}

