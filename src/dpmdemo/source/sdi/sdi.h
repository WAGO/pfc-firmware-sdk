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
///  \file     sdi_command.h
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef DAL_COMMAND_H_
#define DAL_COMMAND_H_

#define ICM_UNUSED_TASK_ID         0x55


#define INIT_DAL                    0
#define EXIT_DAL                    0
#define SCAN_DEVICE                 2
#define GET_DEVICE_LIST             3
#define OPEN_DEVICE                 4
#define CONF_SUBDEVICE              9
#define CONF_DEVICE                 10
#define WRITE_START                 11
#define WRITE_BIT                   12
#define WRITE_BYTE                  13
#define WRITE_END                   14
#define READ_START                  15
#define READ_BIT                    16
#define READ_BYTE                   17
#define READ_END                    18
#define DIAGNOSE_DEVICE             19
#define DIAGNOSE_SUBDEVICE          20
#define GET_LAST_ERROR              21
#define SET_APP_STATE               22
#define WDG_SET_TIME                23
#define WDG_START                   24
#define WDG_STOP                    25
#define WDG_TRIGGER                 26
#define WDG_AUTO                    27
#define BATCH                       28

#define MAX_DIAGS_PER_PAGE          (ICM_MAX_STRINGS - 21)
#define DIAG_OFFSET_PRINT           11
#define DIAG_DEVICE_PROJECTED       0x01
#define DIAG_DEVICE_ACTIVE          0x02
#define DIAG_DEVICE_AVAILABLE       0x04


#define MAX_CROSS_ENTRY             10
#define CROSS_VALUES                6




/* macros */

#define INIT_STRINGS(tmpCommandNameString,commandNameString)                          \
            memset(&tmpCommandNameString[0],0,strlen(tmpCommandNameString));          \
            strncat(tmpCommandNameString,commandNameString,strlen(commandNameString))

#define INIT_AND_COPY_STRINGS(tmpCommandNameString,commandNameString,keyString)       \
            memset(&tmpCommandNameString[0],0,strlen(tmpCommandNameString));          \
            strncat(tmpCommandNameString,commandNameString,strlen(commandNameString)); \
            strncat(tmpCommandNameString,keyString,1)

#define PRINTOUT_ERROR_TEXT(tmpCommandNameString,commandNameString)                  \
        INIT_STRINGS(tmpCommandNameString,commandNameString);                         \
        VT100_SET_CURSOR_EDITOR_HOME();                                               \
        MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,tmpCommandNameString,VT100_NORMAL);   \
        menu_OutputEditorText("/>Error : wrong key entered",                           \
                                  EDITOR_CURSOR_X_HOME_POS + 2,                       \
                                  EDITOR_CURSOR_Y_HOME_POS + 1)

#define PRINT_ERROR_LINE()                                                        \
         menu_ClearEditorLine(1);                                                   \
         VT_100_SAVE_CURSOR_POS();                                                 \
         menu_OutputEditorText("/>Error : wrong key entered",                       \
                                  EDITOR_CURSOR_X_HOME_POS + 2,                    \
                                  EDITOR_CURSOR_Y_HOME_POS + 1);                   \
         VT_100_RESTORE_CURSOR_POS()


#define MENU_PRINTOUT(...) \
         fprintf(stdout, __VA_ARGS__);      \
         fflush(stdout);                    \


void sdi_Init(uint8_t batch);
void sdi_ExitDal(uint8_t batch);
void sdi_ScanDevices(uint8_t batch);
void sdi_GetDeviceList(uint8_t batch);
void sdi_OpenDevice(uint8_t batch);
void sdi_CloseDevice(uint8_t batch);
void sdi_GetDeviceName(uint8_t batch);
void sdi_ConfigureSubdevice(uint8_t batch);
void sdi_ConfigureDevice(uint8_t batch);
void sdi_WriteStart(uint8_t batch);
void sdi_WriteBit(uint8_t batch);
void sdi_WriteBytes(uint8_t batch);
void sdi_WriteEnd(uint8_t batch);
void sdi_ReadStart(uint8_t batch);
void sdi_ReadBit(uint8_t batch);
void sdi_ReadBytes(uint8_t batch);
void sdi_ReadEnd (uint8_t batch);
void sdi_DiagnoseGetDeviceState(uint8_t batch);
void sdi_DiagnoseGetSubdeviceState(uint8_t batch);
void sdi_SetApplicationState(uint8_t batch);
void sdi_Quit(uint8_t batch);

void sdi_Automatic(void);

#endif /* DAL_COMMAND_H_ */
//---- End of source file ------------------------------------------------------
