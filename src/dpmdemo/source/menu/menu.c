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
///  \file     sdi_console_menu.c
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <rs> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <adi_application_interface.h>
#include <profibus_master.h>
#include <dpm_demo.h>
#include <sdi.h>
#include <device_type.h>

#include "menu.h"






//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

static tMenuHead menuHead =
{         0,0,
         {
          "++++++++++++++++++++++++++++++++++++++++++++",
          "+ DAL interactive console menu             +",
          "++++++++++++++++++++++++++++++++++++++++++++",
          "+ WAGO Kontakttechnik GmbH & Co. KG        +",
          "++++++++++++++++++++++++++++++++++++++++++++"
         }
};

tMenu menu[] =
{
  {"+ Init DAL                                 +", false, sdi_Init },
  {"+ Scan Devices                             +", false, sdi_ScanDevices },
  {"+ Get Device List                          +", false, sdi_GetDeviceList },
  {"+ Open Device                              +", false, sdi_OpenDevice  },
  {"+ Get Device Name                          +", false, sdi_GetDeviceName },
  {"+ Configure Subdevice                      +", false, sdi_ConfigureSubdevice },
  {"+ Configure Device                         +", false, sdi_ConfigureDevice },
  {"+ Set Application State                    +", false, sdi_SetApplicationState },
  {"+ Write Start                              +", false, sdi_WriteStart },
  {"+ Write Bit                                +", false, sdi_WriteBit },
  {"+ Write Bytes                              +", false, sdi_WriteBytes },
  {"+ Write End                                +", false, sdi_WriteEnd },
  {"+ Read Start                               +", false, sdi_ReadStart },
  {"+ Read Bit                                 +", false, sdi_ReadBit },
  {"+ Read Bytes                               +", false, sdi_ReadBytes },
  {"+ Read End                                 +", false, sdi_ReadEnd },
  {"+ Diagnose Get Device State                +", false, sdi_DiagnoseGetDeviceState },
  {"+ Diagnose Get Subdevice State             +", false, sdi_DiagnoseGetSubdeviceState },
  {"+ Close Device                             +", false, sdi_CloseDevice },
  {"+ Exit DAL                                 +", false, sdi_ExitDal },
  {"+ Quit Console                             +", false, sdi_Quit },
  { "",false, NULL }
};

static tMenuFoot menuFoot =
{          0,0,
        { "++++++++++++++++++++++++++++++++++++++++++++\0"}
};


tKeyString astKeyStrings[] =
{
  { "CURSOR_UP",      CURSOR_UP_ID,     "[A"    },
  { "CURSOR_LEFT",    CURSOR_LEFT_ID,   "[D"    },
  { "CURSOR_DOWN",    CURSOR_DOWN_ID,   "[B"    },
  { "CURSOR_RIGHT",   CURSOR_RIGHT_ID,  "[C"    },
  { "INSERT",         INSERT_ID,        "[2~"   },
  { "DELETE",         DELETE_ID,        "[3~"   },
  { "POS_1",          POS_1_ID,         "[1~"   },
  { "END",            END_ID,           "[4~"   },
  { "PAGE_UP",        PAGE_UP_ID,       "[5~"   },
  { "PAGE_DOWN",      PAGE_DOWN_ID,     "[6~"   },
  { "",               NO_ID,            ""      }       // this line must always be the last one
};



tBatchProcessing batchProcessing =
{
  .printOutput  = true,
  .cancel       = false,
  .dataCrossing = false
};


tMenuOrg menuOrg;
tBatchProcessing *batchData;
//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


//-- Function: menu_GetMenuInterface --------------------------------------------------------------------
///
///  This function returns a reference to the menu struct.
///
///  \param  entry        index of the array struct
///
///  \return  tMenu *menu
///
//------------------------------------------------------------------------------------------
tMenu *menu_GetMenuInterface(uint8_t entry)
{
   return &menu[entry];
}

//-- Function: menu_GetMenuOrgInterface --------------------------------------------------------------------
///
///  This function returns a reference to the menu organization struct.
///
///  \param  None
///
///  \return  tMenuOrg *menuOrg
///
//------------------------------------------------------------------------------------------
tMenuOrg *menu_GetMenuOrgInterface(void)
{
   return &menuOrg;
}

//-- Function: menu_GetBatchData --------------------------------------------------------------------
///
///  This function returns a reference to batch processing data.
///
///  \param  None
///
///  \return  tBatchProcessing *batchProcessing
///
//------------------------------------------------------------------------------------------
tBatchProcessing *menu_GetBatchData(void)
{
   return &batchProcessing;
}


//-- Function: menu_Close -----------------------------------------------------------------
///
///  This function closes the whole application
///
/// \param        none
///
///  \return      none
//------------------------------------------------------------------------------------------
void menu_Close (void)
{
  VT100_CLEAR_SCREEN();
  VT100_SET_CURSOR_HOME();
  VT100_RESET_ATTRIBUTES();

}



//-- Function: menu_GetUserInput -------------------------------------------------------------
///
///  This function gets the user input from the keyboard
///
///  \param  *inputKey      pressed key
///
///  \return  key hit index
///
//------------------------------------------------------------------------------------------
int32_t menu_GetUserInput(char* inputKey)
{
  int32_t returnValue   = SUCCESS;

  static struct termios backupTty;
  static struct termios newTty;


  // change steam buffer to be able to read a single key
  // first save actual attributes...
  if(tcgetattr(STDIN_FILENO, &backupTty) != SUCCESS)
  {
    returnValue = ERROR;
  }
  else
  {
    // ...and then do the changes
    newTty            = backupTty;
    newTty.c_lflag    = newTty.c_lflag & ~(uint32_t)(ECHO|ICANON|ISIG);
    // read() shall return after at least 2 characters read OR after 100ms
    // (Work-around for the problem when read() returns only a part of a
    // multi-byte escape sequence)
    newTty.c_cc[VMIN] = 2;
    newTty.c_cc[VTIME] = 1;

    if((tcsetattr(STDIN_FILENO, TCSAFLUSH, &newTty)) != SUCCESS)
    {
      returnValue = ERROR;
    }
    else
    {
      if(read(STDIN_FILENO, inputKey, 1) < 1)
      {
        returnValue = ERROR;
      }
      else
      {
        // if normal key was hit, just return its ascii-code
        if(*inputKey == ESCAPE )
        {
          char charBuffer[10];
          int  keyStringIndex = 0;

          // read the next character(s) from keyboard, add end-of-string
          int charCount = read(STDIN_FILENO, charBuffer, 4);
          charBuffer[charCount] = 0;


          // loop over all stored keys in our list until we can identify the read escape-sequence
          while( (astKeyStrings[keyStringIndex].keyId != 0  ) && (returnValue == 0))
          {
            if(strcmp(charBuffer, astKeyStrings[keyStringIndex].keyString) == SUCCESS)
            {
              // key was found - store its id in output-variable
              returnValue = astKeyStrings[keyStringIndex].keyId;
            }
            ++keyStringIndex;
          }

          if(returnValue == 0)
          {// key is unknown, return error
            returnValue = ERROR;
          }
        }
        else if(*inputKey == RETURN)
        {
          returnValue = RETURN;
        }
      }
    }
    // restore original terminal
    if((tcsetattr(STDIN_FILENO, TCSAFLUSH, &backupTty)) != SUCCESS)
    {
      returnValue = ERROR;
    }
  }

  return(returnValue);
}


//-- Function: menu_RefreshMenuSelection -----------------------------------------------------
///
///  This function changes the menu style
///
///  \param  *inLine      menu text line
///  \param  y            vertical position
///  \param  textStyle    normal or inverted mode
///
///  \return  none
///
//------------------------------------------------------------------------------------------
void menu_RefreshMenuSelection(const char* inLine, uint8_t y, uint8_t textStyle)
{

  VT100_SET_CURSOR_TO_XY(MENU_X_COORD,y);

  if(textStyle == INVERS)
  {
    MENU_PRINTOUT("%s%s%s \n", VT100_INVERSE, inLine, VT100_NORMAL);
  }
  else
  {
    MENU_PRINTOUT("%s",inLine);
  }

  return;
}

//-- Function: menu_OutputEditorText ---------------------------------------------------------
///
///  This function prints out a string at the specified editor position
///
///  \param  *inLine      menu text line
///  \param  x            horizontal position
///  \param  y            vertical position
///
///  \return  none
///
//------------------------------------------------------------------------------------------
void menu_OutputEditorText(const char* inLine, uint8_t x, uint8_t y)
{
  size_t iLen;
  iLen = strlen(inLine);

  if( iLen > (EDITOR_LINE_LENGTH - 1) )
  {
    return;
  }
  VT100_SET_CURSOR_TO_XY(x,y);
  menu_ClearEditorLine(y-EDITOR_CURSOR_Y_HOME_POS);
  MENU_PRINTOUT("%s",inLine);
  VT100_SET_CURSOR_TO_XY(x+iLen+1, y);

  return;

}

//-- Function: menu_OutputEditorInversText ----------------------------------------------------
///
///  This function prints out a string at the specified editor position
///
///  \param  *inLine  string to be printed out
///  \param  x        horizontal position
///  \param  y        vertical position
///
///  \return  none
///
//------------------------------------------------------------------------------------------
void menu_OutputEditorInversText(const char* inLine, uint8_t x, uint8_t y)
{
  size_t iLen;
  iLen = strlen(inLine);

  if( iLen > (EDITOR_LINE_LENGTH - 1) )
  {
    return;
  }
  VT100_SET_CURSOR_TO_XY(x,y);

  menu_ClearEditorLine(y-EDITOR_CURSOR_Y_HOME_POS);

  MENU_PRINTOUT("%s%s%s \n", VT100_INVERSE, inLine, VT100_NORMAL);

  VT100_SET_CURSOR_TO_XY(x+iLen+1, y);

  return;

}

//-- Function: menu_PrintMenuHead -----------------------------------------------------------
///
///  This function prints out the menu head lines
///
///  \param  none
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
void menu_PrintMenuHead(void)
{
  uint8_t lineIndex = 0;

  VT100_TEXT_GREEN();                 //text color bright green

  for(lineIndex = 0; lineIndex < menuHead.items; lineIndex++)
  {
    menu_OutputEditorText(&menuHead.lines[lineIndex][0], 2, (uint8_t)(lineIndex + 1) );
  }

  VT100_TEXT_GRAY();                 //restore text color gray
}

//-- Function: menu_PrintMenuLines -----------------------------------------------------------
///
///  This function prints out the selection menu lines
///
///  \param  none
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
void menu_PrintMenuLines(void)
{
  uint8_t lineIndex = 0;

  menu_RefreshMenuSelection(&menu[0].lines[0], menuOrg.offset, INVERS);

  for(lineIndex = 1; lineIndex < menuOrg.items; lineIndex++)
  {
    menu_RefreshMenuSelection(&menu[lineIndex].lines[0], (uint8_t)(menuOrg.offset + lineIndex), NORMAL );
  }

}

//-- Function: menu_PrintMenuFoot -----------------------------------------------------------
///
///  This function prints out the footer lines
///
///  \param  none
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
void menu_PrintMenuFoot(void)
{
  uint8_t lineIndex = 0;

  for(lineIndex = 0; lineIndex < menuFoot.items; lineIndex++)
  {
    menu_OutputEditorText(&menuFoot.lines[lineIndex][0], 2, (uint8_t)(menuFoot.offset + lineIndex) );
  }

}


//-- Function: menu_PrintEditor -----------------------------------------------------------
///
///  This function prints out a collection of new menu fracments
///
///  \param  *string                      new menu text line
///  \param  adiResponse                  device return code
///  \param  cmd                          what has to be done
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
int32_t menu_PrintEditor(char *string, int32_t adiResponse, uint32_t cmd)
{
  tDevice *device = device_GetTypeInterface();
  uint8_t i;
  int32_t returnValue = false;

  switch(cmd)
  {

    case ICM_EDITOR_PRINT_HEAD:
    {
      const char title[EDITOR_LINE_LENGTH]    = "+                       EDITOR                  +";
      const char boundary[EDITOR_LINE_LENGTH] = "+++++++++++++++++++++++++++++++++++++++++++++++++\0";

      VT100_TEXT_GREEN();                 //text color bright green
      menu_OutputEditorText(boundary, EDITOR_CURSOR_X_HOME_POS, (uint8_t)(menuHead.items-2) );
      menu_OutputEditorInversText(title,EDITOR_CURSOR_X_HOME_POS, (uint8_t)(menuHead.items - 1) );
      VT100_TEXT_GREEN();                 //text color bright green
      menu_OutputEditorText(boundary, EDITOR_CURSOR_X_HOME_POS, menuHead.items);
      VT100_TEXT_GRAY();                 //restore text color gray
    }
      break;

    case ICM_REFRESH_EDITOR:
    {
      const char boundary[EDITOR_LINE_LENGTH] = "+++++++++++++++++++++++++++++++++++++++++++++++++\0";
      const char space[EDITOR_LINE_LENGTH]    = "+                                               +\0";
      uint32_t editorHeight;

      for(editorHeight = 0; editorHeight < menuOrg.items; editorHeight++)
      {
        menu_OutputEditorText(space,EDITOR_CURSOR_X_HOME_POS, (uint8_t)(menuOrg.offset + editorHeight) );
      }

      menu_OutputEditorText(boundary, EDITOR_CURSOR_X_HOME_POS, (uint8_t)(menuOrg.offset + menuOrg.items) );


      menu_OutputEditorText("/> ", EDITOR_CURSOR_X_HOME_POS+2, menuOrg.offset);

      VT100_SET_CURSOR_EDITOR_HOME();
      VT_100_SAVE_CURSOR_POS();
      VT100_SET_CURSOR_EDITOR_HOME();
    }
      break;

    case ICM_EDITOR_PRINT_EXECUTION:

      VT100_SET_CURSOR_EDITOR_HOME();
      MENU_PRINTOUT("%s %s  %s",VT100_INVERSE,string,VT100_NORMAL);

      if(adiResponse != DAL_FAILURE)
      {
        menu_OutputEditorText("/>Execution: success", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
        returnValue++;
      }
      else
      {
        menu_OutputEditorText("/>Execution: failed", \
                                  EDITOR_CURSOR_X_HOME_POS + 2,\
                                  EDITOR_CURSOR_Y_HOME_POS + 1);
      }
      break;

    case ICM_EDITOR_PRINT_DEVICE_TABLE:

      if(device->NumberOfDevices == 0)
      {
        VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS+2,\
                               EDITOR_CURSOR_Y_HOME_POS + 2);
        MENU_PRINTOUT("No devices installed");
      }
      else
      {
        menu_OutputEditorText(" |---------------------------------|\0", \
                                 EDITOR_CURSOR_X_HOME_POS+2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 2);
        menu_OutputEditorText(" | Device ID |     Device Name     |\0",\
                                 EDITOR_CURSOR_X_HOME_POS+2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 3);
        menu_OutputEditorText(" |---------------------------------|\0",\
                                 EDITOR_CURSOR_X_HOME_POS+2,\
                                 EDITOR_CURSOR_Y_HOME_POS + 4);

        for(i = 0;i < device->NumberOfDevices; i++)         // loop thru device list
        {
        	menu_OutputEditorText("|\0",								\
        			EDITOR_CURSOR_X_HOME_POS + 37,	\
        			EDITOR_CURSOR_Y_HOME_POS + 5 + i);

          VT100_SET_CURSOR_TO_XY( EDITOR_CURSOR_X_HOME_POS+2,
                                     EDITOR_CURSOR_Y_HOME_POS + 5 + i);
          MENU_PRINTOUT(" |   %d       | %s ", device->info[i].DeviceId, device->info[i].DeviceName);

      /*    menu_OutputEditorText("|\0", \
                                   EDITOR_CURSOR_X_HOME_POS + 37,\
                                   (uint8_t)(EDITOR_CURSOR_Y_HOME_POS + 5 + i) );
      */
        }
        menu_OutputEditorText(" |---------------------------------|\0", \
                                 EDITOR_CURSOR_X_HOME_POS+2,\
                                 (uint8_t)(EDITOR_CURSOR_Y_HOME_POS + 5 + i) );

        menu_OutputEditorText("/> ", \
                                 EDITOR_CURSOR_X_HOME_POS+2, \
                                 (uint8_t)(EDITOR_CURSOR_Y_HOME_POS + 6 + i) );

        VT100_SET_CURSOR_TO_XY( EDITOR_CURSOR_X_HOME_POS+4,\
                                   EDITOR_CURSOR_Y_HOME_POS + 6 + i);

        VT_100_SAVE_CURSOR_POS();

      }
      break;

    case ICM_EDITOR_CLEAR_LINES:
      VT100_SET_CURSOR_EDITOR_HOME();
      MENU_PRINTOUT("                                            ");               // clear first line
      VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 1,\
                                EDITOR_CURSOR_Y_HOME_POS + 1);
      MENU_PRINTOUT("                                            ");               // clear second line

      break;

    default:
      break;


  }

  return returnValue;
}

//-- Function: menu_ClearEditorLine ---------------------------------------------------------
///
///  This function clears a whole editor line
///
///  \param  line                    line to be deteted
///
///  \return  none
///
//------------------------------------------------------------------------------------------
void menu_ClearEditorLine(uint32_t line)
{

  VT_100_SAVE_CURSOR_POS();
  VT100_SET_CURSOR_TO_XY(EDITOR_CURSOR_X_HOME_POS + 1,\
                            EDITOR_CURSOR_Y_HOME_POS + line);
  MENU_PRINTOUT("                                            ");               // clear first line
  // restore cursor position
  VT_100_RESTORE_CURSOR_POS();
}

//-- Function: menu_Print --------------------------------------------------------------
///
/// Show a selection menu with title, headline, footer and the specified menu-lines.
/// The user makes a selection by using Cursor-up and -down and return.
///
///  \param  none
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
int32_t menu_Print(void)
{
  int32_t returnValue   = SUCCESS;
  uint8_t loopString;

  memset(&menuOrg,0, sizeof(menuOrg));                        // init menu org struct
  VT100_RESET_ATTRIBUTES();                                  // reset old attribute modes

  menuHead.offset     = 1;
  menuHead.items = sizeof(menuHead.lines)/ sizeof(menuHead.lines[0]);
  menuOrg.offset  = (uint8_t)(menuHead.offset + menuHead.items);
        //search last menu line
  for(loopString=0; loopString < ICM_MAX_STRINGS; loopString++)
  {
    if(strcmp(menu[loopString].lines,"") == ZERO )
    {
      break;
    }
    else if(loopString == (ICM_MAX_STRINGS - 1) )
    {
      return DAL_FAILURE;
    }
  }
  menuOrg.items  = loopString;

  menuFoot.offset         = (uint8_t)(menuOrg.offset + menuOrg.items);
  menuFoot.items     = sizeof(menuFoot.lines)/ sizeof(menuFoot.lines[0]);

  VT100_CLEAR_SCREEN();

  menu_PrintMenuHead();
  menu_PrintMenuLines();
  menu_PrintMenuFoot();

  menu_PrintEditor("dummy",true, ICM_EDITOR_PRINT_HEAD);
  menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
  return(returnValue);
}


//-- Function: menu_ProcessMenuAction -------------------------------------------------------
///
///  This function shows the new focused menu line
///
///  \param  actualSelection              old selected menu line
///  \param  newSelection                 new selected menu line
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
static int32_t menu_ProcessMenuAction(uint8_t actualSelection, uint8_t newSelection)
{
  tMenuOrg *menuOrg = menu_GetMenuOrgInterface();
  tMenu *menu   = menu_GetMenuInterface(actualSelection);

  if(menu->inverse == false)
  {
      // restore normal text mode
      menu_RefreshMenuSelection(&menu->lines[0],(uint8_t)(actualSelection + menuOrg->offset), NORMAL);
  }

  menuOrg->newSelectedLine = newSelection;

  (actualSelection < newSelection)                ? \
  (menu += (newSelection - actualSelection))  : \
  (menu -= (actualSelection - newSelection));

  // print new selected line number inverse
  menu_RefreshMenuSelection(&menu->lines[0],(uint8_t)(newSelection + menuOrg->offset), INVERS);

  return SUCCESS;
}

//-- Function: menu_Manual ----------------------------------------------------------
///
///  This function catches the user input and forces the execution
///
///  \param  none
///
///  \return  error value
///
//------------------------------------------------------------------------------------------
int32_t menu_Manual(void)
{
  int32_t returnValue       = SUCCESS;
  char key                 = 0;

  batchData                 = menu_GetBatchData();

  while(batchData->cancel == false)
  {

    switch(menu_GetUserInput(&key))
    {
      case CURSOR_UP_ID:    // move selection-bar up (if it is not already in first position)
                            if(menuOrg.actualSelectedLine >  0)
                            {
                              menu_ProcessMenuAction(menuOrg.actualSelectedLine, --menuOrg.newSelectedLine);
                            }
                            break;

      case CURSOR_DOWN_ID:  // move selection-bar down (if it is not already in last position)
                            if(menuOrg.actualSelectedLine < menuOrg.items -1 )
                            {
                              menu_ProcessMenuAction(menuOrg.actualSelectedLine, ++menuOrg.newSelectedLine);
                            }
                            break;

      case PAGE_UP_ID:      // move selection-bar to the first position
      case POS_1_ID:
                            menuOrg.newSelectedLine = 0;
                            menu_ProcessMenuAction(menuOrg.actualSelectedLine, menuOrg.newSelectedLine);
                            break;

      case PAGE_DOWN_ID:    //move selection-bar to the last position
      case END_ID:
                            menuOrg.newSelectedLine = (uint8_t)(menuOrg.items-1);
                            menu_ProcessMenuAction(menuOrg.actualSelectedLine, menuOrg.newSelectedLine);
                            break;

      case 'q':             // quit the menu without selection
      case 'Q':             batchData->cancel = true;
                            break;

      case RETURN:        // execute the actual selected option
                            menu[menuOrg.actualSelectedLine].pFunc(ICM_MANUAL_EXEC);
                            break;

      case DELETE_ID:     // clear editor
                          menu_PrintEditor("dummy",true, ICM_REFRESH_EDITOR);
                          break;

      default:
                          break;
    }
    menuOrg.actualSelectedLine = menuOrg.newSelectedLine;
  }

  VT100_CLEAR_SCREEN();
  VT100_SET_CURSOR_HOME();

  return(returnValue);
}



//---- End of source file ------------------------------------------------------
