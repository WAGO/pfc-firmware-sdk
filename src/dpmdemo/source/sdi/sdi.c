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
///  \file     sdi_command.c
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <rs> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <stdbool.h>
#include <adi_application_interface.h>
#include <dpm_demo.h>
#include <menu.h>
#include <profibus_master.h>
#include <device_type.h>

#include "sdi.h"



tBatchProcessing *batchData;



//-- Function: sdi_Init --------------------------------------------------------
///
///  This function processes the adi init function.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_Init(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tMenu *menu                       = menu_GetMenuInterface(INIT_DAL);
  int32_t adiResponse              = DAL_SUCCESS;

  adiResponse = adi->Init();                      // call adi interface
  if(batch == false)
  {
    menu_PrintEditor("dummy", adiResponse, ICM_REFRESH_EDITOR);
    menu_PrintEditor("Init DAL", adiResponse, ICM_EDITOR_PRINT_EXECUTION);

    menu->inverse = true;                       // set inverse indicator
  }


}

//-- Function: sdi_ExitDal --------------------------------------------------------
///
///  This function processes the adi exit function.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ExitDal(uint8_t batch)
{
  tApplicationDeviceInterface * adi   = adi_GetApplicationInterface();
  tMenuOrg *menuOrg                 = menu_GetMenuOrgInterface();
  tMenu *menu                         = menu_GetMenuInterface(INIT_DAL);
  int32_t adiResponse                = DAL_SUCCESS;
  uint8_t yPos;

  adiResponse = adi->Exit();
  if(batch == false)
  {
    menu_PrintEditor("dummy", adiResponse, ICM_REFRESH_EDITOR);
    menu_PrintEditor("Exit DAL", adiResponse, ICM_EDITOR_PRINT_EXECUTION);

    // restore normal text mode of "Init" item
    yPos = (uint8_t) (menuOrg->offset + INIT_DAL);
    menu_RefreshMenuSelection(&menu[INIT_DAL].lines[0],yPos, NORMAL );
    menu[INIT_DAL].inverse = false;

    // restore normal text mode of "Scan Devices" item
      yPos = (uint8_t) (menuOrg->offset + SCAN_DEVICE);
    menu_RefreshMenuSelection(&menu[SCAN_DEVICE].lines[0],yPos, NORMAL );
    menu[SCAN_DEVICE].inverse = false;
  }


}

//-- Function: sdi_ScanDevices -------------------------------------------------
///
///  This function processes a scan of stored device libraries.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ScanDevices(uint8_t batch)
{
  tApplicationDeviceInterface * adi   = adi_GetApplicationInterface();
  tMenu *menu                         = menu_GetMenuInterface(SCAN_DEVICE);
  int32_t adiResponse                = DAL_SUCCESS;

  adiResponse = adi->ScanDevices();
  if(batch == false)
  {
    menu_PrintEditor("dummy", adiResponse, ICM_REFRESH_EDITOR);
    menu_PrintEditor("Scan Devices", adiResponse, ICM_EDITOR_PRINT_EXECUTION);

    menu->inverse = true;                      // set inverse indicator
  }


}

//-- Function: sdi_GetDeviceList -----------------------------------------------
///
///  This function displays all stored device libraries.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_GetDeviceList(uint8_t batch)
{
   tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
   tDevice *device                   = device_GetTypeInterface();
   int32_t adiResponse              = DAL_SUCCESS;

   adiResponse = adi->GetDeviceList(ELEMENT_COUNT(device->info),
                                    device->info,
                                    &device->NumberOfDevices );

   device_InitSystem();      // setup device description

   if(batch == false)
   {
     menu_PrintEditor("dummy", adiResponse, ICM_REFRESH_EDITOR);
     menu_PrintEditor("Get Device List", adiResponse, ICM_EDITOR_PRINT_EXECUTION);

     if(adiResponse == DAL_SUCCESS)
     {
       menu_PrintEditor("dummy",true, ICM_EDITOR_PRINT_DEVICE_TABLE);
     }
   }

}

//-- Function: sdi_OpenDevice --------------------------------------------------
///
///  This function opens the communication channel to a dedicated device.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_OpenDevice(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Open Device ";
  char tmpCommandNameString[20]     = {0};
  char key;
  int32_t deviceId                  = 0;
  uint8_t quit = false;

  if(batch == true)
  {
    deviceId    = device_getIdFromDeviceName("libpbdpm");
    adiResponse = adi->OpenDevice(deviceId);
    return;
  }
                               // clear editor
  menu_PrintEditor("dummy",adiResponse, ICM_REFRESH_EDITOR);
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,commandNameString,VT100_NORMAL);

  while( quit == false )
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2 );

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("ICM_ERROR while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <= 0x39) )
    {
      deviceId = (char)(key - 0x30);                         // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                   // echo user action

      adiResponse = adi->OpenDevice(deviceId);

      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
      MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
      if(adiResponse == DAL_SUCCESS)
      {
        device->type[deviceId-1].open = true;
      }
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }
}

//-- Function: sdi_CloseDevice -------------------------------------------------
///
///  This function closes the communication channel of a dedicated device.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_CloseDevice(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Close Device ";
  char tmpCommandNameString[20]     = {0};
  int32_t deviceId                  = 0;
  char key;
  uint8_t quit = false;

  if(batch == true)
  {
    deviceId    = device_getIdFromDeviceName("libpbdpm");
    adiResponse = adi->CloseDevice(deviceId);
    return;
  }

                                                   // clear editor
  menu_PrintEditor("dummy",adiResponse, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

  while( quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2 );

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (char)(key - 0x30);                         // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                   // echo user action

      adiResponse = adi->CloseDevice(deviceId);

      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
      MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
      if(adiResponse == DAL_SUCCESS)
      {
        device->type[deviceId-1].open = false;
      }
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }
}

//-- Function: sdi_Init --------------------------------------------------------
///
///  This function dispays the name of a device.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_GetDeviceName(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse              = DAL_SUCCESS;
  const char commandNameString[]   = "Get Device Name ";
  char tmpCommandNameString[20]    = {0};
  char deviceNameBuffer[30]        = {0};
  size_t deviceNameLen;
  char key;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

                                           // clear editor
  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

  while( quit == false )
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }

    if( (key >= 0x30) && (key <=0x39) )
    {
      MENU_PRINTOUT("%d",key-0x30);                   // echo user action

      adiResponse = adi->GetDeviceName((key - 0x30),sizeof(deviceNameBuffer),\
                                       deviceNameBuffer, &deviceNameLen );

      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
      // append device id
      MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);
      menu_ClearEditorLine(1);
      menu_ClearEditorLine(3);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
      if(adiResponse == DAL_SUCCESS)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                       EDITOR_CURSOR_Y_HOME_POS + 3);

        MENU_PRINTOUT("/>Device Name: %s ",deviceNameBuffer);
      }
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 4);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
      menu_ClearEditorLine(3);
    }
  }

}


//-- Function: sdi_Init --------------------------------------------------------
///
///  This function configures a subdevice (slave) below a device (master).
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ConfigureSubdevice(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Configure Subdevice ";
  char tmpCommandNameString[25]     = {0};
  char userInputData[3]             = {0};
  int32_t deviceId;
  int32_t subDeviceId = 0;
  int32_t retVal = SUCCESS;
  uint8_t formattedConfigData[244] = {0};
  uint32_t inputConfigData = 0;
  uint8_t configDataIndex = 0;
  void *configData;
  char key;
  uint8_t loop = 0;
  uint8_t quit = false;
  uint8_t stopInputSequence = false;

  if(batch == true)
  {
    dpm_ConfigureSubdevice();

   return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

                                                            //get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
     VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                               EDITOR_CURSOR_Y_HOME_POS + 1);
     MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (char)(key - 0x30);                         // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                         // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
      quit = true;
      break;
    }
    else
    {                                                         // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                                         // get subdevice id
  {
    menu_OutputEditorText("/>Enter subdevice id:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 3);

    for(loop = 0; loop < 3; loop++)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
       MENU_PRINTOUT("Error while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
        break;
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
      else if( (key >= 0x30) && (key <=0x39) )
      {
        userInputData[loop]= key;
        MENU_PRINTOUT("%d",key-0x30);               // echo user action
        menu_ClearEditorLine(1);                     // clear error line
      }
      else
      {                 // entered unexpected key
        loop--;
        PRINT_ERROR_LINE();
      }
    }
    subDeviceId = strtol(&userInputData[0],NULL,10);
    memset(&userInputData,0,sizeof(userInputData));   // clear input array
  }

  if(quit == false)                           // get config data
  {
    menu_OutputEditorText("/>Insert config data. End with 'e'.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 4);

    for(configDataIndex = 0; configDataIndex < sizeof(formattedConfigData); \
        configDataIndex++)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
      MENU_PRINTOUT("/>Enter config data byte %d:",configDataIndex+1);

      for(loop = 0; loop < 3; loop++ )
      {
        retVal = menu_GetUserInput(&key);
        if(retVal == ERROR)
        {
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 1);
         MENU_PRINTOUT("Error while retrieving input key.\n");
        }
        else if(retVal == RETURN)
        {
          break;
        }
        else if ((key == 'q') || (key == 'Q'))
        {
          menu_OutputEditorText("/>Return to main menu.", \
                                    EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 6);
          quit = true;
          break;
        }
        else if( (key >= 0x30) && (key <=0x39) )
        {
          userInputData[loop]= key;
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 30 + loop,\
                                    EDITOR_CURSOR_Y_HOME_POS + 5);
          MENU_PRINTOUT("%d",key-0x30);                 // echo user action
          menu_ClearEditorLine(1);                      // clear error line
        }
        else if (key == 'e')
        {
          stopInputSequence = true;
          break;
        }
        else
        {                 // entered unexpected key
          loop--;
          PRINT_ERROR_LINE();
        }

      }
      if((quit == false) && (stopInputSequence == false) )
      {
        inputConfigData = (uint32_t)strtol(&userInputData[0],NULL,10);
        if((inputConfigData > 255) || (inputConfigData == 0))
        {
          configDataIndex--;
        }
        else
        {
          formattedConfigData[configDataIndex] = (uint8_t)inputConfigData;
        }
        memset(&userInputData,0,sizeof(userInputData));
        menu_ClearEditorLine(1);                     // clear error line
        menu_ClearEditorLine(5);
      }
      if( (quit == true) || (stopInputSequence == true) )
      {
        break;
      }
    }
  }

  if(quit == false)                 // process command
  {

    adiResponse = adi->ConfigureSubdevice(deviceId, subDeviceId, &configData);
    menu_ClearEditorLine(1);
    menu_ClearEditorLine(4);
    menu_ClearEditorLine(5);

    menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    menu_OutputEditorText("/>Return to main menu.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 5);

  }
}

//-- Function: sdi_ConfigureDevice ---------------------------------------------
///
///  This function configures a device which is physically present on the pfc.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ConfigureDevice(uint8_t batch)
{

  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse              = DAL_SUCCESS;
  const char commandNameString[]   = "Configure Device ";
  int32_t retVal                   = SUCCESS;

  char tmpCommandNameString[25]    = {0};
  char userInputData[6]            = {0};
  int32_t deviceId;
  uint32_t inputConfigData         = 0;
  uint8_t configDataIndex          = 0;
  void *configData;
  char key;
  uint8_t loop = 0;
  uint8_t quit = false;
  uint8_t stopInputSequence = false;


  if(batch == true)
  {
    dpm_ConfigureDevice();

   return;
  }
  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

  /***************  user input mode ************************/
  while(quit == false)
  {

    menu_OutputEditorText("/>Enter device id or (q)uit :", \
                              EDITOR_CURSOR_X_HOME_POS + 2, \
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )                     // get device id
    {
      deviceId = (char)(key - 0x30);                        // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                        // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                           // get config data
  {
      menu_OutputEditorText("/>Insert config data. End with 'e'.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);

    for(configDataIndex = 0; configDataIndex < 244; \
        configDataIndex++)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 4);
      MENU_PRINTOUT("/>Enter config data byte %d:",configDataIndex+1);

      for(loop = 0; loop < 6; loop++ )
      {
        retVal = menu_GetUserInput(&key);
        if(retVal == ERROR)
        {
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 1);
         MENU_PRINTOUT("ICM_ERROR while retrieving input key.\n");
        }
        else if(retVal == RETURN)
        {
          break;
        }
        else if ((key == 'q') || (key == 'Q'))
        {
          menu_OutputEditorText("/>Return to main menu.", \
                                    EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 5);
          quit = true;
          break;
        }
        else if( (key >= 0x30) && (key <=0x39) )
        {
          userInputData[loop]= key;
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 30 + loop,\
                                    EDITOR_CURSOR_Y_HOME_POS + 4);
          MENU_PRINTOUT("%d",key-0x30);                   // echo user action
          menu_ClearEditorLine(1);                        // clear ICM_ERROR line
        }
        else if (key == 'e')
        {
          stopInputSequence = true;
          break;
        }
        else
        {                                                   // entered unexpected key
          loop--;
          PRINT_ERROR_LINE();
        }

      }
      if((quit == false) && (stopInputSequence == false) )
      {
        inputConfigData = (uint32_t)strtol(&userInputData[0],NULL,10);
        if(inputConfigData > 0x3FFFFF)
        {
          configDataIndex--;
        }
        else
        {
       //   formattedConfigData[configDataIndex] = inputConfigData;
        }
        memset(&userInputData,0,sizeof(userInputData));
        menu_ClearEditorLine(1);                     // clear ICM_ERROR line
        menu_ClearEditorLine(4);
      }
      if( (quit == true) || (stopInputSequence == true) )
      {
        break;
      }
    }
  }

  if(quit == false)                 // process command
  {
    switch(device->type[deviceId-1].item)
    {
      case PROFIBUS_SLAVE:
      case CANOPEN_SLAVE:
      case CANOPEN_MASTER:
      case MODBUS:
        break;

      default:
        break;
    }

   adiResponse = adi->ConfigureDevice(deviceId, configData);

   menu_ClearEditorLine(1);
   menu_ClearEditorLine(4);

   menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
   menu_OutputEditorText("/>Return to main menu.",           \
                             EDITOR_CURSOR_X_HOME_POS + 2,  \
                             EDITOR_CURSOR_Y_HOME_POS + 4);
  }
}

//-- Function: sdi_WriteStart --------------------------------------------------
///
///  This function must be executed before a write cycle. It prevents inconsistent
///  process data.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_WriteStart(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tMenu *menu                       = menu_GetMenuInterface(WRITE_START);
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Write Start ";
  char tmpCommandNameString[25]     = {0};
  int32_t deviceId;
  char key;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }


  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

                            // get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2 );

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("ICM_ERROR while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = key - 0x30;                          // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                 // echo user action

        //prevent deadlock
      if(device->type[deviceId-1].wlock == TRUE )
      {
        adiResponse = DAL_FAILURE;
      }
      else
      {
        adiResponse = adi->WriteStart(deviceId, ICM_UNUSED_TASK_ID);
        device->type[deviceId-1].wlock = TRUE;
//        if(adiResponse == DAL_SUCCESS)
//        {
//          if(device->type.name[deviceId-1].item  == KBUS)
//          {
//            sdi_createKbusUpdateThread();
//          }
//        }
      }

      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }
  menu->inverse = true;                       // set inverse indicator

}

//-- Function: sdi_WriteBit ----------------------------------------------------
///
///  This function writes a single bit.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_WriteBit(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();


  int32_t adiResponse              = DAL_SUCCESS;
  const char commandNameString[]  = "Write Bit ";
  char tmpCommandNameString[25]   = {0};
  char userInputData[10]          = {0};
  int32_t deviceId                = 0;
  uint8_t  writeBoolVal             = 0;
  uint32_t bitOffset               = 0;
  int32_t retVal                   = SUCCESS;
  char key;
  uint8_t loop = 0;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }


  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

                            // get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
     MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (int32_t)(key - 0x30);                    // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                        // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 6);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                   // get bitoffset
  {
    menu_OutputEditorText("/>Enter bitoffset:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 3);

    for(loop = 0; loop < 10; loop++)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
        MENU_PRINTOUT("ERROR while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
        break;
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
      else if( (key >= 0x30) && (key <=0x39) )
      {
        userInputData[loop]= key;
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 22 + loop,\
                                  EDITOR_CURSOR_Y_HOME_POS + 3);
        MENU_PRINTOUT("%d",key-0x30);                 // echo user action
        menu_ClearEditorLine(1);                      // clear error line
      }
      else
      {                 // entered unexpected key
        loop--;
        PRINT_ERROR_LINE();
      }
    }
    bitOffset = (uint32_t)strtol(&userInputData[0],NULL,10);
    memset(&userInputData,0,sizeof(userInputData));
  }

                                          // get bit value
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter bit value:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 4);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key == 0x30) || (key ==0x31) )
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 22,\
                                EDITOR_CURSOR_Y_HOME_POS + 4);
      writeBoolVal = (uint8_t)(key - 0x30);
      MENU_PRINTOUT("%d", writeBoolVal);                  // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINT_ERROR_LINE();
    }
  }

  if(quit == false)                 // process command
  {
    adiResponse = adi->WriteBool(deviceId, ICM_UNUSED_TASK_ID, bitOffset, writeBoolVal);

    menu_ClearEditorLine(1);
    menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    menu_OutputEditorText("/>Return to main menu.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 5);
  }
}

//-- Function: sdi_WriteBytes --------------------------------------------------
///
///  This function writes a bunch of process data bytes.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_WriteBytes(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();

  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Write Bytes ";
  char tmpCommandNameString[25]     = {0};
  char formattedWriteData[256]      = {0};
  char userInputData[10]            = {0};
  int32_t deviceId = 0;
  uint32_t byteOffset = 0;
  int32_t retVal = SUCCESS;

  char key;
  uint8_t loop = 0;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

                                  // get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (int32_t)(key - 0x30);            // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 6);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                   // get byte offset
  {
    menu_OutputEditorText("/>Enter byte offset:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 3);

    for(loop = 0; loop < 5; loop++)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
        MENU_PRINTOUT("Error while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
        if(strlen(userInputData) == 0)
        {
          loop--;
        }
        else
        {
          break;
        }
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
      else if( (key >= 0x30) && (key <=0x39) )
      {
        userInputData[loop]= key;
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 23 + loop,\
                                  EDITOR_CURSOR_Y_HOME_POS + 3);
        MENU_PRINTOUT("%d",key-0x30);                 // echo user action
        menu_ClearEditorLine(1);                      // clear error line
      }
      else
      {                 // entered unexpected key
        loop--;
        PRINT_ERROR_LINE();
      }
    }
    byteOffset = (uint32_t)strtol(&userInputData[0],NULL,10);
    memset(&userInputData,0,sizeof(userInputData));
  }


  if(quit == false)                           // get write data
  {
    uint16_t inputWriteData = 0;
    uint16_t writeDataIndex = 0;
    uint8_t stopInputSequence = false;

    menu_OutputEditorText("/>Insert write data. End with 'e'.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 4);

    for(writeDataIndex = 0; writeDataIndex < sizeof(formattedWriteData); writeDataIndex++)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
      MENU_PRINTOUT("/>Enter write data byte %d:",writeDataIndex+1);

      for(loop = 0; loop < 3; loop++ )
      {
        retVal = menu_GetUserInput(&key);
        if(retVal == ERROR)
        {
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 1);
         MENU_PRINTOUT("Error while retrieving input key.\n");
        }
        else if(retVal == RETURN)
        {
          if(loop == 0)
            loop--;
          else
            break;
        }
        else if ((key == 'q') || (key == 'Q'))
        {
          menu_OutputEditorText("/>Return to main menu.", \
                                    EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 6);
          quit = true;
          break;
        }
        else if( (key >= 0x30) && (key <=0x39) )
        {
          userInputData[loop]= key;
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 29 + loop,\
                                    EDITOR_CURSOR_Y_HOME_POS + 5);
          MENU_PRINTOUT("%d",key-0x30);                 // echo user action
        }
        else if (key == 'e')
        {
          stopInputSequence = true;
          break;
        }
        else
        {                 // entered unexpected key
          loop--;
          PRINT_ERROR_LINE();
        }
      }
      if((quit == false) && (stopInputSequence == false) )
      {
        inputWriteData = (uint16_t)strtol(&userInputData[0],NULL,10);
        if((inputWriteData > 255) || (inputWriteData == 0))
        {
          writeDataIndex--;
        }
        else
        {
          formattedWriteData[writeDataIndex] = (uint8_t)inputWriteData;
        }
        memset(&userInputData,0,sizeof(userInputData));
        menu_ClearEditorLine(1);                     // clear error line
        menu_ClearEditorLine(5);
      }
      if( (quit == true) || (stopInputSequence == true) )
      {
        break;
      }
    }
  }

  if(quit == false)                 // process command
  {
    uint32_t writeDataSize;

    writeDataSize = strlen(formattedWriteData);

    adiResponse = adi->WriteBytes (deviceId, ICM_UNUSED_TASK_ID,  \
                                   byteOffset,                    \
                                   writeDataSize,                 \
                                   (uint8_t *)&formattedWriteData[0]);
    menu_ClearEditorLine(1);
    menu_ClearEditorLine(4);
    menu_ClearEditorLine(5);

    menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    menu_OutputEditorText("/>Return to main menu.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 4);
  }
}

//-- Function: sdi_WriteEnd ----------------------------------------------------
///
///  This function must be executed after a write cycle. It cancels the write lock.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_WriteEnd(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tMenuOrg *menuOrg                 = menu_GetMenuOrgInterface();
  tMenu *menu                       = menu_GetMenuInterface(WRITE_START);
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Write End ";
  char tmpCommandNameString[25]     = {0};
  int32_t deviceId;
  char key;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);


  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2 );

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <= 0x39) )
    {
      deviceId = (int32_t)(key - 0x30);            // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                // echo user action

      adiResponse = adi->WriteEnd(deviceId, ICM_UNUSED_TASK_ID);

      device->type[deviceId].wlock = FALSE;

      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
      MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }
  // restore normal text mode "Write Start" item
  menu_OutputEditorText(&menu->lines[0], 2, (uint8_t)(WRITE_START + menuOrg->offset) );
  if(menu->inverse == true)
    menu->inverse = false;                       // reset inverse indicator

}

//-- Function: sdi_ReadStart ----------------------------------------------------
///
///  This function must be executed before a read cycle. It prevents inconsistent
///  process data.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ReadStart(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tMenu *menu                       = menu_GetMenuInterface(READ_START);
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse              = DAL_SUCCESS;
  const char commandNameString[]   = "Read Start ";
  char tmpCommandNameString[25]     = {0};
  int32_t deviceId;
  char key;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);



                                                  // get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2 );

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (int32_t)(key - 0x30);                     // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                        // echo user action

      	  //prevent deadlock
		if(device->type[deviceId].rlock == TRUE )
		{
		  adiResponse = DAL_FAILURE;
		}
		else
		{
		  adiResponse = adi->ReadStart(deviceId, ICM_UNUSED_TASK_ID);
		  device->type[deviceId].rlock = TRUE;
		}


      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
   //   MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }
  menu->inverse = true;                       // set inverse indicator

}

//-- Function: sdi_ReadBit -----------------------------------------------------
///
///  This function reads a single bit.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ReadBit(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse              = DAL_SUCCESS;
  const char commandNameString[]  = "Read Bit ";
  char tmpCommandNameString[25]   = {0};
  char userInputData[10]          = {0};
  int32_t deviceId                = 0;
  uint32_t bitOffset               = 0;
  int32_t retVal                   = SUCCESS;
  char key;
  uint8_t loop = 0;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);



  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
     MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (int32_t)(key - 0x30);             // ASCII convert

      MENU_PRINTOUT("%d",deviceId);                 // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 6);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                   // get bitoffset
  {
    menu_OutputEditorText("/>Enter bitoffset:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 3);

    for(loop = 0; loop < 10; loop++)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
        MENU_PRINTOUT("Error while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
        break;
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
      else if( (key >= 0x30) && (key <=0x39) )
      {
        userInputData[loop]= key;
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 22 + loop,\
                                  EDITOR_CURSOR_Y_HOME_POS + 3);
        MENU_PRINTOUT("%d",key-0x30);                 // echo user action
        menu_ClearEditorLine(1);                      // clear error line
      }
      else
      {                 // entered unexpected key
        loop--;
        PRINT_ERROR_LINE();
      }
    }
    bitOffset = (uint32_t)strtol(&userInputData[0],NULL,10);
    memset(&userInputData,0,sizeof(userInputData));
  }
  if(quit == false)                 // process command
  {
    uint8_t bitMask = 0;
    uint8_t tmpMask = 0;


    adiResponse = adi->ReadBit(deviceId, ICM_UNUSED_TASK_ID, bitOffset, &bitMask);

    menu_ClearEditorLine(1);
    menu_ClearEditorLine(4);
    menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);

    if(adiResponse == DAL_SUCCESS)
    {
      tmpMask = (uint8_t) (1<<(bitOffset % 8));
      if(bitMask ==  tmpMask)
      {
        menu_OutputEditorText("/>Bit value: TRUE", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 4);
      }
      else
      {
        menu_OutputEditorText("/>Bit value: FALSE", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 4);
      }
    }

    menu_OutputEditorText("/>Return to main menu.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 5);
  }

}

//-- Function: sdi_ReadBytes ---------------------------------------------------
///
///  This function reads a bunch of process data bytes.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ReadBytes(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Read Bytes ";
  char tmpCommandNameString[25]     = {0};
  char userInputData[10]            = {0};
  uint8_t editorPrintPosition = 7;
  uint32_t readDataSize  = 0;
  int32_t deviceId      = 0;
  uint32_t byteOffset    = 0;
  int32_t retVal         = SUCCESS;
  char key;
  uint32_t loop = 0;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

                                  // get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (int32_t)(key - 0x30);            // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                // echo user action
       menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 6);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                   // get byte offset
  {
    menu_OutputEditorText("/>Enter byte offset:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 3);

    for(loop = 0; loop < 5; loop++)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
        MENU_PRINTOUT("Error while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
        if(strlen(userInputData) == 0)
        {
          loop--;
        }
        else
        {
          break;
        }
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
      else if( (key >= 0x30) && (key <=0x39) )
      {
        userInputData[loop]= key;
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 23 + loop,\
                                  EDITOR_CURSOR_Y_HOME_POS + 3);
        MENU_PRINTOUT("%d",key-0x30);                 // echo user action
        menu_ClearEditorLine(1);                      // clear error line
      }
      else
      {                 // entered unexpected key
        loop--;
        PRINT_ERROR_LINE();
      }
    }
    byteOffset = (uint32_t)strtol(&userInputData[0],NULL,10);
    memset(&userInputData,0,sizeof(userInputData));
  }

  if(quit == false)                   // get read data size
  {
    menu_OutputEditorText("/>Enter read data size:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 4);

    for(loop = 0; loop < 5; loop++)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
        MENU_PRINTOUT("Error while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
        if(strlen(userInputData) == 0)
        {
          loop--;
        }
        else
        {
          break;
        }
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
      else if( (key >= 0x30) && (key <=0x39) )
      {
        userInputData[loop]= key;
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 26 + loop,\
                                  EDITOR_CURSOR_Y_HOME_POS + 4);
        MENU_PRINTOUT("%d",key-0x30);                 // echo user action
        menu_ClearEditorLine(1);                      // clear error line
      }
      else
      {                 // entered unexpected key
        loop--;
        PRINT_ERROR_LINE();
      }
    }
    readDataSize = (uint32_t)strtol(&userInputData[0],NULL,10);
    memset(&userInputData,0,sizeof(userInputData));
  }

  if(quit == false)                 // process command
  {
    uint32_t i;
    uint8_t *readBuffer;
    uint8_t *memBuffer;

    memBuffer = (uint8_t *)safe_malloc(readDataSize);
  //  memBuffer = (uint8_t *)malloc(readDataSize);
    if(memBuffer == NULL)
    {
      menu_OutputEditorText("/>Out of memory.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 6);
      return;
    }
    memset(memBuffer, 0, readDataSize);
    readBuffer = memBuffer;

    adiResponse = adi->ReadBytes (deviceId, ICM_UNUSED_TASK_ID, byteOffset,\
                                  readDataSize, readBuffer);

    menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    if(adiResponse == DAL_SUCCESS)
    {
      menu_OutputEditorText("/>Print out read data <RETURN> or (q)uit:", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);
    }
    else
    {
      quit = true;
    }

    while(quit == false)
    {
      retVal = menu_GetUserInput(&key);
      if(retVal == ERROR)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
       MENU_PRINTOUT("Error while retrieving input key.\n");
      }
      else if(retVal == RETURN)
      {
                                                     // printout a page of read data
        for(loop = 0; loop < readDataSize;)
        {
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + editorPrintPosition++);
          MENU_PRINTOUT("/>Byte %d = ",loop );
          MENU_PRINTOUT("0x%hx, ",*readBuffer++); loop++;
          MENU_PRINTOUT("0x%hx, ",*readBuffer++); loop++;
          MENU_PRINTOUT("0x%hx, ",*readBuffer++); loop++;
          MENU_PRINTOUT("0x%hx",  *readBuffer++); loop++;

          if(editorPrintPosition >= MAX_EDITOR_MENU_LINES)
          {
            quit = false;
            editorPrintPosition = 7;

            while(quit == false)
            {
              retVal = menu_GetUserInput(&key);
              if(retVal == ERROR)
              {
                VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                          EDITOR_CURSOR_Y_HOME_POS + 1);
               MENU_PRINTOUT("Error while retrieving input key.\n");
              }
              else if(retVal == RETURN)
              {
                for(i = 6; i < MAX_EDITOR_MENU_LINES; i++)
                {
                  menu_ClearEditorLine(i);                  // clear editor page
                }
                quit = true;
              }
              else if( (key == 'q') || (key == 'Q') )
              {
                menu_OutputEditorText("/>Return to main menu.", \
                                          EDITOR_CURSOR_X_HOME_POS + 2,\
                                          EDITOR_CURSOR_Y_HOME_POS + 6);
                quit = true;
                break;
              }
            }
          }
        }

        break;
      }
      else if( (key == 'q') || (key == 'Q') )
      {
        menu_OutputEditorText("/>Return to main menu.", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 6);
        quit = true;
        break;
      }
    }
    if(memBuffer != NULL)
    {
      free(memBuffer);
      memBuffer = NULL;
    }
    menu_OutputEditorText("/>Return to main menu.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 6);
  }
}

//-- Function: sdi_ReadEnd -----------------------------------------------------
///
///  This function must be executed after a read cycle. It cancels the read lock.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_ReadEnd (uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  tMenuOrg *menuOrg               = menu_GetMenuOrgInterface();
  tMenu *menu                       = menu_GetMenuInterface(READ_START);
  tDevice *device                   = device_GetTypeInterface();
  int32_t adiResponse              = DAL_SUCCESS;
  const char commandNameString[]   = "Read End ";
  char tmpCommandNameString[25]     = {0};
  int32_t deviceId;
  char key;
  uint8_t quit = false;

  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);


  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2 );

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <= 0x39) )
    {
      deviceId = (int32_t)(key - 0x30);                    // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                        // echo user action

      adiResponse = adi->ReadEnd(deviceId, ICM_UNUSED_TASK_ID);

      device->type[deviceId].wlock = FALSE;

      VT100_SET_CURSOR_EDITOR_HOME();
      INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString, &key);
      MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 3);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }
  // restore normal text mode "Write Start" item
  menu_OutputEditorText(&menu->lines[0], 2, (uint8_t)(READ_START + menuOrg->offset) );
  if(menu->inverse == true)
    menu->inverse = false;                       // reset inverse indicator
}

//-- Function: sdi_DiagnoseGetDeviceState --------------------------------------
///
///  This function displays the diagnostic topology data of a specific device.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_DiagnoseGetDeviceState(uint8_t batch)
{

  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse               = DAL_SUCCESS;
  const char commandNameString[]   = "Diagnose Get Device State ";
  char tmpCommandNameString[25]     = {0};
  uint8_t diagBuffer[256]           =  {0};
  int32_t deviceId                 = 0;
  uint32_t pageCounter              = 0;
  int32_t retVal                    = SUCCESS;
  char key;
  uint32_t i;
  uint8_t quit                      = false;
  uint8_t noop                      = false;
  tBusState *busState               = NULL;
  char diagAvailable[]              = "false";
  char deviceActive[]               = "false";
  char deviceProjected[]            = "false";


  if(batch == true)
  {
    return;
  }

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  INIT_STRINGS(tmpCommandNameString,commandNameString);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);


                                  // get device id
  while(quit == false)
  {
    menu_OutputEditorText("/>Enter device id or (q)uit:", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 2);

    if(menu_GetUserInput(&key) == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    if( (key >= 0x30) && (key <=0x39) )
    {
      deviceId = (int32_t)(key - 0x30);             // ASCII convert
      MENU_PRINTOUT("%d",deviceId);                 // echo user action
      menu_ClearEditorLine(1);
      break;
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 4);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
    }
  }

  if(quit == false)                 // process command
  {
    adiResponse = adi->DiagnoseGetDeviceState (deviceId, sizeof(diagBuffer), \
                                               diagBuffer,busState);

    menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);

    if(adiResponse == DAL_FAILURE)
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 4);
    }
    else
    {
      menu_OutputEditorText("/>Navigate with arrow key or (q)uit:", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 5);

      menu_OutputEditorText("|---------------------------------------------|", \
                               EDITOR_CURSOR_X_HOME_POS+1,\
                               EDITOR_CURSOR_Y_HOME_POS + 6);
      menu_OutputEditorText("|                   Devices                   |", \
                               EDITOR_CURSOR_X_HOME_POS+1,\
                               EDITOR_CURSOR_Y_HOME_POS + 7);
      menu_OutputEditorText("|---------------------------------------------|", \
                               EDITOR_CURSOR_X_HOME_POS+1,\
                               EDITOR_CURSOR_Y_HOME_POS + 8);
      menu_OutputEditorText("| Address  | Diagnose  | Active  | Projected  |",\
                               EDITOR_CURSOR_X_HOME_POS+1,\
                               EDITOR_CURSOR_Y_HOME_POS + 9);
      menu_OutputEditorText("|---------------------------------------------|", \
                               EDITOR_CURSOR_X_HOME_POS+1,\
                               EDITOR_CURSOR_Y_HOME_POS + 10);


      while(quit == false)
      {
        if(noop == false)
        {
                                              // clear old editor content
          for(i = DIAG_OFFSET_PRINT; i < (MAX_DIAGS_PER_PAGE + DIAG_OFFSET_PRINT); i++)
          {
            menu_ClearEditorLine(i);
          }
                                       // print out actual diag page
          for(i = 0; (i < MAX_DIAGS_PER_PAGE)
                   && ((pageCounter*MAX_DIAGS_PER_PAGE)+i) < sizeof(diagBuffer);
              i++)
          {
            if( (diagBuffer[(pageCounter*MAX_DIAGS_PER_PAGE)+i] & DIAG_DEVICE_PROJECTED)
                == DIAG_DEVICE_PROJECTED)    // subdevice is present
            {
              INIT_STRINGS(deviceProjected , "true");
            }
            else
            {
              INIT_STRINGS(deviceProjected , "false");
            }
            if( (diagBuffer[(pageCounter*MAX_DIAGS_PER_PAGE)+i] & DIAG_DEVICE_ACTIVE)
                == DIAG_DEVICE_ACTIVE)    // subdevice is active
            {
              INIT_STRINGS(deviceActive , "true");
            }
            else
            {
              INIT_STRINGS(deviceActive , "false");
            }
            if( (diagBuffer[(pageCounter*MAX_DIAGS_PER_PAGE)+i] & DIAG_DEVICE_AVAILABLE)
                == DIAG_DEVICE_AVAILABLE)    // subdevice has diagnosis
            {
              INIT_STRINGS(diagAvailable , "true");
            }
            else
            {
              INIT_STRINGS(diagAvailable , "false");
            }

            VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                      EDITOR_CURSOR_Y_HOME_POS + DIAG_OFFSET_PRINT + i);

            MENU_PRINTOUT("  %d        %s       %s      %s",(pageCounter*MAX_DIAGS_PER_PAGE) + i,
                   diagAvailable, deviceActive, deviceProjected);
          }
        }
        retVal = menu_GetUserInput(&key);

        if(retVal == ERROR)
        {
          VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 1);
         MENU_PRINTOUT("Error while retrieving input key.\n");
         break;
        }
        else if( (retVal == CURSOR_UP_ID) )
        {
          if(pageCounter >= 1)
          {
            pageCounter--;
            noop = false;
            menu_ClearEditorLine(4);
          }
          else
          {
            menu_OutputEditorText("/>Reached diagnosis border", \
                                      EDITOR_CURSOR_X_HOME_POS + 2,\
                                      EDITOR_CURSOR_Y_HOME_POS + 4);
            noop = true;
          }
        }
        else if( (retVal == CURSOR_DOWN_ID) )
        {
          if( ((pageCounter+1)*MAX_DIAGS_PER_PAGE) <= sizeof(diagBuffer) )
          {
            pageCounter++;
            noop = false;
            menu_ClearEditorLine(4);
          }
          else
          {
            menu_OutputEditorText("/>Reached diagnosis border", \
                                      EDITOR_CURSOR_X_HOME_POS + 2,\
                                      EDITOR_CURSOR_Y_HOME_POS + 4);
            noop = true;
          }
        }
        else if( (key == 'q') || (key == 'Q') )
        {
          menu_OutputEditorText("/>Return to main menu.", \
                                    EDITOR_CURSOR_X_HOME_POS + 2,\
                                    EDITOR_CURSOR_Y_HOME_POS + 4);
          quit = true;
          break;
        }
      }//while(quit == false)
    }
    menu_OutputEditorText("/>Return to main menu.", \
                              EDITOR_CURSOR_X_HOME_POS + 2,\
                              EDITOR_CURSOR_Y_HOME_POS + 4);
  }
}

//-- Function: sdi_DiagnoseGetSubdeviceState -----------------------------------
///
///  This function displays the diagnostic data of a specific subdevice.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_DiagnoseGetSubdeviceState(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
   int32_t adiResponse               = DAL_SUCCESS;
   const char commandNameString[]   = "Diagnose Get Subdevice State ";
   char tmpCommandNameString[25]     = {0};
   uint8_t diagBuffer[256]           =  {0};
   char userInputData[3]             = {0};
   int32_t deviceId                 = 0;
   int32_t subDeviceId              = 0;
   uint32_t pageCounter              = 0;
   uint8_t loop                      = 0;
   int32_t retVal                    = SUCCESS;
   char key;
   uint32_t i;
   uint8_t quit                      = false;
   uint8_t noop                      = false;
   tDiagnoseState *diagnoseState     = NULL;

   if(batch == true)
   {
     return;
   }

   menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
   INIT_STRINGS(tmpCommandNameString,commandNameString);
   VT100_SET_CURSOR_EDITOR_HOME();
   MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);

                                   // get device id
   while(quit == false)
   {
     menu_OutputEditorText("/>Enter device id or (q)uit:", \
                               EDITOR_CURSOR_X_HOME_POS + 2,\
                               EDITOR_CURSOR_Y_HOME_POS + 2);

     if(menu_GetUserInput(&key) == ERROR)
     {
       VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 1);
       MENU_PRINTOUT("Error while retrieving input key.\n");
     }
     if( (key >= 0x30) && (key <=0x39) )
     {
       deviceId = (int32_t)(key - 0x30);                    // ASCII convert
       MENU_PRINTOUT("%d",deviceId);                        // echo user action
       menu_ClearEditorLine(1);
       break;
     }
     else if( (key == 'q') || (key == 'Q') )
     {
       menu_OutputEditorText("/>Return to main menu.", \
                                 EDITOR_CURSOR_X_HOME_POS + 2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 4);
       quit = true;
       break;
     }
     else
     {                 // entered unexpected key
       PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString);
     }
   }

   if(quit == false)                   // get subdevice id
   {
     menu_OutputEditorText("/>Enter subdevice id:", \
                               EDITOR_CURSOR_X_HOME_POS + 2,\
                               EDITOR_CURSOR_Y_HOME_POS + 3);

     for(loop = 0; loop < 3; loop++)
     {
       retVal = menu_GetUserInput(&key);
       if(retVal == ERROR)
       {
         VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                   EDITOR_CURSOR_Y_HOME_POS + 1);
        MENU_PRINTOUT("Error while retrieving input key.\n");
       }
       else if(retVal == RETURN)
       {
         break;
       }
       else if( (key == 'q') || (key == 'Q') )
       {
         menu_OutputEditorText("/>Return to main menu.", \
                                   EDITOR_CURSOR_X_HOME_POS + 2,\
                                   EDITOR_CURSOR_Y_HOME_POS + 6);
         quit = true;
         break;
       }
       else if( (key >= 0x30) && (key <=0x39) )
       {
         userInputData[loop]= key;
         MENU_PRINTOUT("%d",key-0x30);                 // echo user action
         menu_ClearEditorLine(1);                      // clear error line
       }
       else
       {                 // entered unexpected key
         loop--;
         PRINT_ERROR_LINE();
       }
     }
     subDeviceId = strtol(&userInputData[0],NULL,10);
   }

   if(quit == false)                 // process command
   {
     adiResponse = adi->DiagnoseGetSubdeviceState (deviceId, subDeviceId,  \
                                                   sizeof(diagBuffer),    \
                                                   diagBuffer,             \
                                                   diagnoseState);

     menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);

     if(adiResponse == DAL_FAILURE)
     {
       menu_OutputEditorText("/>Return to main menu.", \
                                 EDITOR_CURSOR_X_HOME_POS + 2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 4);
     }
     else
     {
       menu_OutputEditorText("/>Navigate with arrow key or (q)uit:", \
                                 EDITOR_CURSOR_X_HOME_POS + 2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 5);

       menu_OutputEditorText("|---------------------------------------------|", \
                                EDITOR_CURSOR_X_HOME_POS+1,\
                                EDITOR_CURSOR_Y_HOME_POS + 6);
       menu_OutputEditorText("|                                             |", \
                                EDITOR_CURSOR_X_HOME_POS+1,\
                                EDITOR_CURSOR_Y_HOME_POS + 7);
       memset(&tmpCommandNameString[0],0,sizeof(tmpCommandNameString));
       strcat(tmpCommandNameString,"Subdevice ");
       strncat(tmpCommandNameString,userInputData,strlen(userInputData));
       menu_OutputEditorText(tmpCommandNameString, \
                                EDITOR_CURSOR_X_HOME_POS+16,\
                                EDITOR_CURSOR_Y_HOME_POS + 7);
       menu_OutputEditorText("|---------------------------------------------|", \
                                EDITOR_CURSOR_X_HOME_POS+1,\
                                EDITOR_CURSOR_Y_HOME_POS + 8);
       menu_OutputEditorText("|    Byte index     |    Content              |", \
                                EDITOR_CURSOR_X_HOME_POS+1,\
                                EDITOR_CURSOR_Y_HOME_POS + 9);
       menu_OutputEditorText("|---------------------------------------------|", \
                                EDITOR_CURSOR_X_HOME_POS+1,\
                                EDITOR_CURSOR_Y_HOME_POS + 10);

       while(quit == false)
       {
         if(noop == false)
         {
                                               // clear old editor content
           for(i = DIAG_OFFSET_PRINT; i < (MAX_DIAGS_PER_PAGE + DIAG_OFFSET_PRINT); i++)
           {
             menu_ClearEditorLine(i);
           }
                                        // print out actual diag page
           for(i = 0; (i < MAX_DIAGS_PER_PAGE)
                    && ((pageCounter*MAX_DIAGS_PER_PAGE)+i) < sizeof(diagBuffer);
               i++)
           {
             VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                       EDITOR_CURSOR_Y_HOME_POS + DIAG_OFFSET_PRINT + i);

             MENU_PRINTOUT("   %d                  0x%x", (pageCounter*MAX_DIAGS_PER_PAGE) + i, \
                              diagBuffer[(pageCounter*MAX_DIAGS_PER_PAGE)+i]);
          }
         }
         retVal = menu_GetUserInput(&key);

         if(retVal == ERROR)
         {
           VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                     EDITOR_CURSOR_Y_HOME_POS + 1);
          MENU_PRINTOUT("Error while retrieving input key.\n");
          break;
         }
         else if( (retVal == CURSOR_UP_ID) )
         {
           if(pageCounter >= 1)
           {
             pageCounter--;
             noop = false;
             menu_ClearEditorLine(4);
           }
           else
           {
             menu_OutputEditorText("/>Reached diagnosis border", \
                                       EDITOR_CURSOR_X_HOME_POS + 2,\
                                       EDITOR_CURSOR_Y_HOME_POS + 4);
             noop = true;
           }
         }
         else if( (retVal == CURSOR_DOWN_ID) )
         {
           if( ((pageCounter+1)*MAX_DIAGS_PER_PAGE) <= sizeof(diagBuffer) )
           {
             pageCounter++;
             noop = false;
             menu_ClearEditorLine(4);
           }
           else
           {
             menu_OutputEditorText("/>Reached diagnosis border", \
                                       EDITOR_CURSOR_X_HOME_POS + 2,\
                                       EDITOR_CURSOR_Y_HOME_POS + 4);
             noop = true;
           }
         }
         else if( (key == 'q') || (key == 'Q') )
         {
           menu_OutputEditorText("/>Return to main menu.", \
                                     EDITOR_CURSOR_X_HOME_POS + 2,\
                                     EDITOR_CURSOR_Y_HOME_POS + 4);
           quit = true;
           break;
         }
       }//if(quit == false)                // process command
     }
     menu_OutputEditorText("/>Return to main menu.", \
                               EDITOR_CURSOR_X_HOME_POS + 2,\
                               EDITOR_CURSOR_Y_HOME_POS + 4);
   }
}

//-- Function: sdi_SetApplicationState -----------------------------------------
///
///  This function sets a device into a specific application state.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------
void sdi_SetApplicationState(uint8_t batch)
{
  tApplicationDeviceInterface * adi = adi_GetApplicationInterface();
  int32_t adiResponse              = DAL_SUCCESS;
  char commandNameString[]   = "Set Application State ";
  char tmpCommandNameString[20]     = {0};
  int32_t retVal                    = SUCCESS;
  char key;
  uint8_t quit = false;
  uint8_t stateIndex = 1;
  tApplicationStateChangedEvent appStateChanged;
  char applicationState[3][20]  = {"Running","Stopped","Unconfigured"};

  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  VT100_SET_CURSOR_EDITOR_HOME();
  MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,commandNameString,VT100_NORMAL);

  if(batch == true)
  {
    appStateChanged.State      = ApplicationState_Unconfigured;
    adi->ApplicationStateChanged( appStateChanged);
    appStateChanged.State      = ApplicationState_Stopped;
    adi->ApplicationStateChanged(appStateChanged);
    appStateChanged.State      = ApplicationState_Running;
    adi->ApplicationStateChanged(appStateChanged);

    return;
  }

  menu_OutputEditorText("/>Navigate with arrow key or (q)uit.",  \
                            EDITOR_CURSOR_X_HOME_POS + 2,     \
                            EDITOR_CURSOR_Y_HOME_POS + 5);

  menu_OutputEditorText("/>Set application state: ", \
                            EDITOR_CURSOR_X_HOME_POS + 2,     \
                            EDITOR_CURSOR_Y_HOME_POS + 6);

  menu_OutputEditorText(applicationState[stateIndex-1],   \
                         EDITOR_CURSOR_X_HOME_POS + 28,     \
                         EDITOR_CURSOR_Y_HOME_POS + 6);

	appStateChanged.State = ApplicationState_Running;

  while(quit == false)                 // process command
  {
    retVal = menu_GetUserInput(&key);
    if(retVal == ERROR)
    {
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("Error while retrieving input key.\n");
    }
    else if(retVal == RETURN)
    {
      adiResponse = adi->ApplicationStateChanged(appStateChanged);
      menu_ClearEditorLine(4);
      menu_ClearEditorLine(1);
      menu_PrintEditor(tmpCommandNameString, adiResponse, ICM_EDITOR_PRINT_EXECUTION);
    }
    else if( (retVal == CURSOR_UP_ID) )
    {
      menu_ClearEditorLine(1);
      if(stateIndex > 1)
      {
        stateIndex--;
        appStateChanged.State = stateIndex;

        menu_ClearEditorLine(4);
                                                      // clear previous command
        menu_OutputEditorText("                  ",   \
                               EDITOR_CURSOR_X_HOME_POS + 28,     \
                               EDITOR_CURSOR_Y_HOME_POS + 6);

        menu_OutputEditorText(applicationState[stateIndex-1],   \
                               EDITOR_CURSOR_X_HOME_POS + 28,     \
                               EDITOR_CURSOR_Y_HOME_POS + 6);
      }
      else
      {
        menu_OutputEditorText("/>No more application states", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 4);
      }
    }
    else if( (retVal == CURSOR_DOWN_ID) )
    {
      menu_ClearEditorLine(1);
      if( stateIndex < (sizeof(applicationState) / sizeof(applicationState[0])) )
      {
        stateIndex++;
        appStateChanged.State = stateIndex;
        menu_ClearEditorLine(4);
                                          // clear previous command
        menu_OutputEditorText("                  ",   \
                                  EDITOR_CURSOR_X_HOME_POS + 28,     \
                                  EDITOR_CURSOR_Y_HOME_POS + 6);

        menu_OutputEditorText(applicationState[stateIndex-1],        \
                                 EDITOR_CURSOR_X_HOME_POS + 28,     \
                                 EDITOR_CURSOR_Y_HOME_POS + 6);
      }
      else
      {
        menu_OutputEditorText("/>No more application states", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 4);
      }
    }
    else if( (key == 'q') || (key == 'Q') )
    {
      menu_OutputEditorText("/>Return to main menu.", \
                                EDITOR_CURSOR_X_HOME_POS + 2,\
                                EDITOR_CURSOR_Y_HOME_POS + 4);
      quit = true;
      break;
    }
    else
    {                 // entered unexpected key
      PRINT_ERROR_LINE();
      menu_ClearEditorLine(3);
    }
  }
}



void sdi_Automatic(void)
{
  int8_t i;

  tMenu *menu                       = menu_GetMenuInterface(INIT_DAL);

  for (i = 0; i < 8; i++)
  {
    menu[i].pFunc(true);         // call menu function
  }

}


//-- Function: sdi_Quit --------------------------------------------------------------------
///
///  This function processes the adi init function.
///
///  \param  batch     process function in batch context
///  \param  *values   values out of batch script
///
///  \return  None
///
//------------------------------------------------------------------------------------------
void sdi_Quit(uint8_t batch)
{
  batchData                         = menu_GetBatchData();
  tApplicationStateChangedEvent     appStateChanged;
  tDeviceId deviceId;
  int32_t adiResponse               = DAL_SUCCESS;

  UNUSED_PARAMETER(batch);

  batchData->cancel = true;

  deviceId    = device_getIdFromDeviceName("libpbdpm");

  appStateChanged.State      = ApplicationState_Stopped;
  adi_GetApplicationInterface()->ApplicationStateChanged(appStateChanged);

  appStateChanged.State      = ApplicationState_Unconfigured;
  adi_GetApplicationInterface()->ApplicationStateChanged( appStateChanged);


  adiResponse = adi_GetApplicationInterface()->CloseDevice(deviceId);
  if ( adiResponse == DAL_SUCCESS)
  {
    printf("Close Device %d success.\n", deviceId);
  }
  else
  {
    printf("Close Device %d failed.\n", deviceId);
  }


  adi_GetApplicationInterface()->Exit();




#if 0

  for(i=0; i < device->NumberOfDevices; i++)
  {
    if(device->type[i].open == true)
    {
      deviceId = device->info[i].DeviceId;
      adiResponse = adi_GetApplicationInterface()->CloseDevice(deviceId);
      if ( adiResponse == DAL_SUCCESS)
      {
        printf("Close Device %d success.\n", deviceId);
      }
      else
      {
        printf("Close Device %d failed.\n", deviceId);
      }
    }
  }
#endif

  usleep(200000);     // device closing down time



  menu_Close();

}


//---- End of source file ------------------------------------------------------
