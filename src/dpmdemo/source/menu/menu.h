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
///  \file     dal_console_menu.h
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef DAL_CONSOLE_MENU_H_
#define DAL_CONSOLE_MENU_H_


// error-numbers
#define MISSING_PARAMETER                -2
#define INVALID_PARAMETER                -3

// editor menu commands
#define ICM_EDITOR_PRINT_EXECUTION        1
#define ICM_EDITOR_PRINT_DEVICE_TABLE     2
#define ICM_EDITOR_PRINT_HEAD             3
#define ICM_REFRESH_EDITOR                4
#define ICM_EDITOR_CLEAR_LINES            5
#define ICM_MENU_UP                       6
#define ICM_MENU_DOWN                     7
#define ICM_MAX_STRINGS                   40
#define ICM_MAX_STRING_SIZE               50
#define ICM_QUIT                          ICM_ERROR
#define ICM_AUTO_EXEC                     1
#define ICM_MANUAL_EXEC                   0

//text style
#define INVERS                            0
#define NORMAL                            1

//coordinates
#define MENU_X_COORD                      2

//  menu dimension
#define MENU_LINE_LENGTH                 44
#define MAX_MENU_LINES                   42
#define MAX_EDITOR_MENU_LINES            33
#define EDITOR_CURSOR_X_HOME_POS         MENU_LINE_LENGTH + 4
#define EDITOR_CURSOR_Y_HOME_POS         6
#define EDITOR_LINE_LENGTH               50

// ASCII key codes
#define RETURN                           10
#define TAB                              9
#define BACKSPACE                        127
#define DELETE                           183

// VT100 sequences
#define ESC                               "\033"  /*escape char */
#define ESCAPE                             0x01B
#define CSI                               "\x1b["
#define VT100_INVERSE                     "\033[7m"
#define VT100_NORMAL                     "\033[0m"
#define VT_100_SAVE_CURSOR_POS()          MENU_PRINTOUT("\033[s");
#define VT_100_RESTORE_CURSOR_POS()       MENU_PRINTOUT("\033[u");
#define VT100_RESET_ATTRIBUTES()          MENU_PRINTOUT("\x1b[m")                       // reset old attribute modes
#define VT100_TEXT_GREEN()                MENU_PRINTOUT("\x1b[32m")
#define VT100_TEXT_GRAY()                 MENU_PRINTOUT("\x1b[37m")
#define VT100_CLEAR_SCREEN()              MENU_PRINTOUT("\x1b[2J")
#define VT100_SET_CURSOR_HOME()           MENU_PRINTOUT("\x1b[f")
#define VT100_SET_CURSOR_TO_XY(x,y)       MENU_PRINTOUT("\033[%d;%dH",y,x);
#define VT100_SET_CURSOR_EDITOR_HOME()    MENU_PRINTOUT("\033[%d;%dH",EDITOR_CURSOR_Y_HOME_POS,\
                                                                EDITOR_CURSOR_X_HOME_POS+4);





// helper
#define ZERO          0
#define UNUSED_PARAM  0
#ifndef FALSE
# define FALSE        0
#endif
#ifndef TRUE
  #define TRUE        1
#endif

// execution mode
#define MENU_MODE           1
#define BATCH_MODE          2

/* main console menu*/

typedef struct stMenu
{
  const char lines[ICM_MAX_STRING_SIZE];
  uint8_t inverse;
  void (*pFunc)(uint8_t batch);
}tMenu;

typedef struct stMenuHead
{
  uint8_t offset;
  uint8_t items;
  const char lines[5][ICM_MAX_STRING_SIZE];
} tMenuHead;

typedef struct stMenuOrg
{
  uint8_t offset;
  uint8_t items;
  uint8_t newSelectedLine;
  uint8_t actualSelectedLine;
} tMenuOrg;


typedef struct stMenuFoot{
  uint8_t offset;
  uint8_t items;
  const char lines[1][ICM_MAX_STRING_SIZE];
} tMenuFoot;


typedef enum
{
  NO_ID             = 0,
  CURSOR_UP_ID      = 178,
  CURSOR_LEFT_ID,
  CURSOR_DOWN_ID,
  CURSOR_RIGHT_ID,
  INSERT_ID,
  DELETE_ID,
  POS_1_ID,
  END_ID,
  PAGE_UP_ID,
  PAGE_DOWN_ID,
} tKeyId;


typedef struct
{
 char   keyDescription[20];
 tKeyId keyId;
 char   keyString[10];
} tKeyString;


typedef struct stBatchProcessing
{
  uint8_t printOutput;                  // set printf output mode
  uint8_t cancel;                       // set abort mode in batch processing
  uint8_t dataCrossing;
}tBatchProcessing;


int32_t menu_Manual(void);
void menu_Close (void);
tMenu *menu_GetMenuInterface(uint8_t entry);
tMenuOrg *menu_GetMenuOrgInterface(void);
tBatchProcessing *menu_GetBatchData(void);
void menu_RefreshMenuSelection(const char* inLine, uint8_t y, uint8_t textStyle);
void menu_OutputEditorText(const char* inLine, uint8_t x, uint8_t y);
void menu_OutputEditorInversText(const char* inLine, uint8_t x, uint8_t y);
int32_t menu_GetUserInput(char* inputKey);
int32_t menu_PrintEditor(char *string, int32_t adiResponse, uint32_t cmd);
int32_t menu_Print(void);
void menu_ClearEditorLine(uint32_t line);

#endif /* DAL_CONSOLE_MENU_H_ */
//---- End of source file ------------------------------------------------------
