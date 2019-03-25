//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2006 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     wdialog.c
///
///  \version  $Revision: 29292 $1
///
///  \brief
///
///  \author   Stefanie Meihöfer : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <unistd.h>
#include <string.h>

#include <termios.h>
#include <wchar.h>
#include <locale.h>

#include <poll.h>

#include <errno.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

// boolean
#define TRUE                             1
#define FALSE                            0

// error-numbers
#define SUCCESS                          0
#define ERROR                            -1
#define MISSING_PARAMETER                -2
#define INVALID_PARAMETER                -3

#define ABORT_SELECTED                   1

#define ESCAPE                           0x01B

#define INFORMATION_AREA                 "information-area"
#define SELECTION_MENU                   "selection-menu"
#define INPUT_WINDOW                     "input-window"

#define LINE_LENGTH                      70
#define MAX_MENU_LINES                   21

// escape-sequences for screen-control
#define INVERS                           "\033[7m"
#define NORMAL                           "\033[0m"
#define BOLD                             "\033[1m"

#define SAVE_CURSOR_POS                  printf("\033[s"); fflush(stdout)
#define RESTORE_CURSOR_POS               printf("\033[u"); fflush(stdout)

// ASCII-Codes from "normal" keys (among 178)
#define RETURN                           10
#define TAB                              9
#define BACKSPACE                        127
#define DELETE                           183

#define CODE_FIRST_PRINTABLE             32
#define LAST_VALID_INPUT_CHAR            127

typedef enum
{
  INPUT = 1,
  OK,
  ABORT

} teSelectedField;



//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

int Help(int    lineCount,
         char** ppLineStrings);

int Clear(int    lineCount,
          char** ppLineStrings);

int MessageBox(int     lineCount,
               char**  ppLineStrings);

int InfoBox(int    lineCount,
            char** ppLineStrings);

int DialogBox(int     lineCount,
               char**  ppLineStrings);

int Menu(int     lineCount,
         char**  ppLineStrings);

int InputBox(int    lineCount,
             char** ppLineStrings);

int PwdInputBox(int    lineCount,
             char** ppLineStrings);

int __InputBox(int     lineCount,
               char** ppLineStrings,
               bool   bPasswordMode);

typedef struct
{
  char* pcFunctionName;
  int   (*pfFunction) (int lineCount, char** ppLineStrings);

} tWindowJumptab;

tWindowJumptab astWindowJumptab[] =
{
  { "--help",      Help },
  { "--clear",     Clear },
  { "--msgbox",    MessageBox },
  { "--infobox",   InfoBox },
  { "--dialogbox", DialogBox },
  { "--menu",      Menu },
  { "--inputbox",  InputBox },
  { "--pwdbox",    PwdInputBox },

  { NULL, NULL }
};



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

  F1_ID,
  F2_ID,
  F3_ID,
  F4_ID,
  F5_ID,
  F6_ID,
  F7_ID,
  F8_ID,
  F9_ID,
  F10_ID,
  F11_ID,
  F12_ID

} tKeyId;


typedef struct
{
  char   keyDescription[20];
  tKeyId keyId;
  char   keyString[10];

} tKeyString;


tKeyString astKeyStrings[] =
{
  { "CURSOR_UP",      CURSOR_UP_ID,     "[A"    },
  { "CURSOR_LEFT",    CURSOR_LEFT_ID,   "[D"    },
  { "CURSOR_DOWN",    CURSOR_DOWN_ID,   "[B"    },
  { "CURSOR_RIGHT",   CURSOR_RIGHT_ID,  "[C"    },

  { "INSERT",         INSERT_ID,        "[2~"   },
  { "DELETE",         DELETE_ID,        "[3~"   },
  { "POS_1",          POS_1_ID,         "[H"    },
  { "END",            END_ID,           "[F"    },
  { "PAGE_UP",        PAGE_UP_ID,       "[5~"   },
  { "PAGE_DOWN",      PAGE_DOWN_ID,     "[6~"   },

  { "F1",             F1_ID,            "OP"    },
  { "F2",             F2_ID,            "OQ"    },
  { "F3",             F3_ID,            "OR"    },
  { "F4",             F4_ID,            "OS"    },
  { "F5",             F5_ID,            "[15~"  },
  { "F6",             F6_ID,            "[17~"  },
  { "F7",             F7_ID,            "[18~"  },
  { "F8",             F8_ID,            "[19~"  },
  { "F9",             F9_ID,            "[20~"  },
  { "F10",            F10_ID,           "[21~"  },
  { "F11",            F11_ID,           "[23~"  },

  { "",               NO_ID,            ""      }       // this line must always be the last one, don't remove it!
};


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions Getkey
//------------------------------------------------------------------------------


int Getkey(void)
//
// Get an input from keyboard without printing it to screen. Escape-Sequenzes where coded.
//
{
  int outKeyId  = ERROR;

  static struct termios backupTty;
  static struct termios newTty;

  do{

    if(tcgetattr(STDIN_FILENO, &backupTty) != SUCCESS)
    {
      break;
    }

    // change steam buffer to be able to read a single key
    // first save actual attributes...
    // ...and then do the changes
    newTty            = backupTty;
    newTty.c_lflag    = newTty.c_lflag & ~(ECHO|ICANON|ISIG);

    if((tcsetattr(STDIN_FILENO, TCSAFLUSH, &newTty)) != SUCCESS)
    {
      break;
    }

    // longest recognized escape sequence is 4 bytes long
    char readSeq[5];
    size_t charSeqIndex;

    // 1st read (blocking)
    if(read(STDIN_FILENO, readSeq, 1) < 1)
    {
      break;
    }

    // subsequent reads to extract multibyte/escape sequences (non-blocking)
    struct pollfd ufds;
    int rv;
    for(charSeqIndex=1; charSeqIndex < sizeof(readSeq); ++charSeqIndex)
    {
      ufds.fd = 0;
      ufds.events = POLLIN;

      if(rv = poll(&ufds, 1, 20) > 0) // poll timeout = 20ms
      {
        if( (rv = read(STDIN_FILENO, readSeq + charSeqIndex, 1)) < 1)
        {
          // Error in read()
          // FIXME: handle?
          // We should regain consistent state anyway by returning
          // outKeyId which is ERROR by now
          break;
        }
      }
      else
      {
        // End of sequence detected or error in poll
        break;
      }
    }

    if(rv < 0)
    {
      break;
    }

    // Terminate readSeq
    readSeq[charSeqIndex] = 0;

    if( 0x1b == readSeq[0] ) // escape {key,sequence}
    {

      int keyStringIndex = 0;
      while((astKeyStrings[keyStringIndex].keyId != NO_ID) && (outKeyId == ERROR))
      {

        if(strcmp(&readSeq[1], astKeyStrings[keyStringIndex].keyString) == SUCCESS)
        {
          // key was found - store its id in output-variable
          outKeyId = astKeyStrings[keyStringIndex].keyId;
          break;
        }
        ++keyStringIndex;

      }
    }
    else
    {
      wchar_t wch;
      mbtowc(&wch, readSeq, sizeof(readSeq) - 1);
      outKeyId = (int) wch;
    }
  } while(0);

// restore original terminal
  if((tcsetattr(STDIN_FILENO, TCSAFLUSH, &backupTty)) != SUCCESS)
  {
    outKeyId = ERROR;
  }

  return(outKeyId);
}


//------------------------------------------------------------------------------
// Local help-functions for Dialog
//------------------------------------------------------------------------------


void SetCursorToXY(int x, int y)
{
  printf("\033[%d;%dH", y, x);
}


void PrintTextLine(char* inLine)
//
// cut line to maximum width (line-length less spaces in front of and behind) and put it to screen
//
{
  if(inLine != NULL)
  {
    char outputLine[LINE_LENGTH + 1] = "";
    strncpy(outputLine, inLine, LINE_LENGTH - 2);
    if(strlen(inLine) > LINE_LENGTH - 2)
    {
      outputLine[LINE_LENGTH - 2] = '~';
    }
    printf(" %s \n", outputLine);
  }
  return;
}

void PrintInversTextLine(char* inLine)
//
// cut line to maximum width (line-length less spaces in front of and behind) and put it to screen in invers colors
//
{
  if(inLine != NULL)
  {
    char outputLine[LINE_LENGTH + 1] = "";
    strncpy(outputLine, inLine, LINE_LENGTH - 2);
    printf(" %s%s %s \n", INVERS, outputLine, NORMAL);
  }
  return;
}


void PrintHyphen(char hyphenChar)
//
// printf horizontal hyphen to screen, consisting of the denoted character
//
{
  char hyphenString[LINE_LENGTH + 1] = "";
  memset(hyphenString, hyphenChar, LINE_LENGTH);
  hyphenString[LINE_LENGTH] = 0;
  printf("%s\n", hyphenString);
}


void PrintTitle(char* pcTitleText)
//
// show title-line on screen
//
{
  PrintHyphen('=');
  PrintTextLine(pcTitleText);
  PrintHyphen('=');
}


void PrintHeadline(char* pcHeadlineText)
//
// Show headline-text on screen
//
{
  PrintTextLine(pcHeadlineText);
  PrintHyphen('-');
}


void PrintFooter(char* pcFooterText)
//
// Show footer with text on screen
//
{
  PrintHyphen('-');
  PrintTextLine(pcFooterText);
  PrintHyphen('-');
}


void PrintBox(int width)
//
// Print borders of a box
// input: inner width of box
//
{
  char borderLineString[LINE_LENGTH + 1]  = "";
  char middleLineString[LINE_LENGTH + 1]  = "";

  // create top and bottom line of box
  memset(borderLineString, '-', LINE_LENGTH);
  borderLineString[0] = '+';
  sprintf(&borderLineString[width + 1], "+");

  // create middle line of box
  sprintf(middleLineString, "|%*s|", width, " ");

  // print the several lines
  PrintTextLine(borderLineString);
  PrintTextLine(middleLineString);
  PrintTextLine(borderLineString);

  return;
}



int PrintWindow(int    lineCount,
                char** ppLineStrings,
                char*  pFooterText)
//
// Print complete window to screen, consisting of title, headline, a different count of text-lines and the footer
//
{
  int lineIndex = 0;

  if(lineCount < 3)
  {
    return(MISSING_PARAMETER);
  }

  Clear(0, NULL);
  PrintTitle(ppLineStrings[0]);
  PrintHeadline(ppLineStrings[1]);

  for(lineIndex = 2; lineIndex < lineCount; ++lineIndex)
  {
    PrintTextLine(ppLineStrings[lineIndex]);
  }

  PrintFooter(pFooterText);

  return(SUCCESS);
}


void ShowErrorText(void)
{
  printf("\n* Display dialog-boxes from shell-skripts *\n\n");
  printf("Aufruf: wdialog <box> [box options]\n\n");
  printf("box / box options:\n");
  printf("--help\n");
  printf("--clear\n");
  printf("--infobox   <title> <headline> <line 1> [line 2-n]\n");
  printf("--msgbox    <title> <headline> <line 1> [line 2-n]\n");
  printf("--dialogbox <title> <headline> <line 1> [line 2-n]\n");
  printf("--menu      <title> <headline> <line 1> [line 2-%d]\n", MAX_MENU_LINES);
  printf("--inputbox  <title> <headline> <input-field-caption> <width> [default]\n");
  printf("\n");
  printf("Width on screen:        %d character\n", LINE_LENGTH);
  printf("Maximum textline-width: %d character\n", LINE_LENGTH - 2);
  printf("Maximum inputbox-width: %d character\n", LINE_LENGTH - 4);
  printf("\n");
  return;
}


//------------------------------------------------------------------------------
// Local main-functions for dialog
//------------------------------------------------------------------------------

int Clear(int    lineCount,
          char** ppLineStrings)
//
// Clears the screen
//
{
  system("clear");
  return(SUCCESS);
}


int Help(int    lineCount,
         char** ppLineStrings)
//
// Show help-text
//
{
  ShowErrorText();

  printf("Description:\n");
  printf("--help      Show this help-text\n");
  printf("--clear     Clears the screen\n");
  printf("--infobox   Show an infobox without confirm\n");
  printf("--msgbox    Show an infobox with confirm\n");
  printf("--dialogbox Show an infobox with OK/Abort confirm\n");
  printf("--menu      Show a selection-menu\n");
  printf("--inputbox  Show an input-box about <width> characters and <default> default-value\n");
  printf("\n");
  return(SUCCESS);
}

int MessageBox(int    lineCount,
               char** ppLineStrings)
//
// Show message-box with title, headline, footer and the specified text-lines.
// Wait until the user wants to quit.
//
{
  int quit      = FALSE;
  PrintWindow(lineCount, ppLineStrings, "Q or RETURN to quit");

  while(quit == FALSE)
  {
    int key = Getkey();
    if((key == L'q') || (key == L'Q') || (key == L'0') || (key == RETURN))
    {
      quit = TRUE;
    }
  }

  return(SUCCESS);
}


int DialogBox(int    lineCount,
               char** ppLineStrings)
//
// Show message-box with title, headline, footer and the specified text-lines.
// Return 0 for abort and 1 for OK.
//
{
  int quit      = FALSE;
  int status    = 0;

  PrintWindow(lineCount, ppLineStrings, "Q for abort or RETURN to accept");

  while(quit == FALSE)
  {
    int key = Getkey();
    if((key == L'q') || (key == L'Q') || (key == L'0'))
    {
      quit = TRUE;
      status =  0;
    }
    else if((key == RETURN) || (key == L'1'))
    {
      quit = TRUE;
      status =  1;
    }
  }

  return status;
}


int InfoBox(int    lineCount,
            char** ppLineStrings)
//
// Show info-box with title, headline, footer and the specified text-lines.
// Don't wait for a reaction of the user.
//
{
  int lineIndex = 0;

  if(lineCount < 3)
  {
    return(MISSING_PARAMETER);
  }

  PrintWindow(lineCount, ppLineStrings, "...please wait");
  return(SUCCESS);
}


int Menu(int     lineCount,
         char**  ppLineStrings)
//
// Show a selection menu with title, headline, footer and the specified menu-lines.
// The user can make a selection by using Cursor-up and -down and return,
// or he can directly type the number of the line.
// Maximum count of options is 10 (0 - 9).
// 0 is always used for "quit", also q and Q
//
{
  int     selectedLineNr  = 0;
  int     menuStartLineNr = 6;
  int     quit            = FALSE;
  int     menuLineCount   = lineCount - 2;;
  char**  ppMenuStrings   = &ppLineStrings[2];
  int     outSelection    = 0;

  // check input-parameter
  if(lineCount < 3)
  {
    return(MISSING_PARAMETER);
  }

  if(lineCount > MAX_MENU_LINES + 2)
  {
    return(INVALID_PARAMETER);
  }

  PrintWindow(lineCount, ppLineStrings, "Select an entry or Q to quit");
  SAVE_CURSOR_POS;

  while(quit == FALSE)
  {
    int key = 0;

    // print the actual selcted line invers
    SetCursorToXY(1, menuStartLineNr + selectedLineNr);
    PrintInversTextLine(ppMenuStrings[selectedLineNr]);
    RESTORE_CURSOR_POS;

    key = Getkey();
    //printf("key:%d selectedLineNr:%d\n", key, selectedLineNr);

    // print the actual selected line normal again, before we evaluate the hit key which may change the selected line
    SetCursorToXY(1, menuStartLineNr + selectedLineNr);
    PrintTextLine(ppMenuStrings[selectedLineNr]);

    switch(key)
    {
      // selection-bar should be moved up (if it is not already in first position)
      case CURSOR_UP_ID:    if(selectedLineNr > 0)
                            {
                              --selectedLineNr;
                            }
                            break;

      // selection-bar should be moved down (if it is not already in last position)
      case CURSOR_DOWN_ID:  if(selectedLineNr < (menuLineCount - 1))
                            {
                              ++selectedLineNr;
                            }
                            break;

      // selection-bar should be moved to first or last position
      case POS_1_ID:        selectedLineNr = 0;
                            break;
      case END_ID:          selectedLineNr = menuLineCount - 1;
                            break;

      // q or Q or cursor-left is hit -> quit the menu without selection
      case 'q':
      case 'Q':
      case CURSOR_LEFT_ID:  outSelection = 0;
                            quit = TRUE;
                            break;

      // return or cursor-right is hit -> quit the menu with the actual selected option
      case RETURN:
      case CURSOR_RIGHT_ID: outSelection = selectedLineNr;
                            quit = TRUE;
                            break;

      // any other key was hit -> evaluate it if it is a valid numeric key, else just refuse it
      default:              // if input was numeric key in the right area, quit the menu with the numeric-selection
                            if((key >= '0' ) && (key < ('0' + menuLineCount)))
                            {
                              outSelection = key - '0';
                              quit = TRUE;
                            }
                            break;
    }
  }

  //fprintf(stderr, "%d", outSelection);
  return(outSelection);
}

// WAT9768 (passwords starting/ending with spaces): clone GetInputString() to prevent breaking "--inputbox"
int GetPasswordString(int   x,
                      int   y,
                      int   width,
                      wchar_t* pDefaultString,
                      wchar_t* pInputString)
//
// Get an input-string from user at a specified area on screen. Spaces in front of and at the end of the input-string
// where cut.
//
// input: x, y:           Coordinates where the input-field starts
//        width:          width of input-field, respectivly maximum length of input-string
//        pDefaultString: String with default value for input-string; NULL, if no default-value is given
//        pInputString:   Pointer to a buffer with input-string (must be allocated by the calling function!)
//
{
  int   quit                    = FALSE;
  int   characterIndex          = 0;
  int   key                     = 0;
  wchar_t aInput[LINE_LENGTH + 1] = L"";
  wchar_t aAsterisks[LINE_LENGTH + 1] = L"";
  int   highestCharIndex = 0;

  if((width + x) > LINE_LENGTH)
  {
    return(INVALID_PARAMETER);
  }

   // initialize the buffer for the single input-characters, first copy the default-string, if it is given
  if(pDefaultString != NULL)
  {
    swprintf(aInput, LINE_LENGTH, L"%ls", pDefaultString);
  }

  // then fill the rest characters with spaces (if necessairy) and add end-of-string
  if(wcslen(aInput) < width)
  {
    swprintf(&aInput[wcslen(aInput)], LINE_LENGTH + 1, L"%*ls", width - wcslen(aInput), L"");
  }
  aInput[width] = 0;

  // evaluate the selected keys and initiate the according changes until the user wants to quit
  quit = FALSE;
  while(quit == FALSE)
  {
    // show actual character-buffer on screen
    SetCursorToXY(x, y);

    // always write full (visible) buffer to delete previous input
    wmemset(aAsterisks, ' ', width);
    wmemset(aAsterisks, '*', highestCharIndex);
    printf("%ls", aAsterisks);
    SetCursorToXY(x + characterIndex, y);
    fflush(stdout);

    key = Getkey();
    switch(key)
    {
      // cursor should be moved one step left (if it is not already in first position)
      case CURSOR_LEFT_ID:  if(characterIndex > 0)
                            {
                              --characterIndex;
                            }
                            break;

      // cursor should be moved one step right (if it is not already in last position)
      case CURSOR_RIGHT_ID: if(characterIndex < (width - 1) && characterIndex < highestCharIndex)
                            {
                              ++characterIndex;
                            }
                            break;

      // cursor should be moved to first or last position
      case POS_1_ID:        characterIndex = 0;
                            break;
      case END_ID:          characterIndex = width - 1;
                            break;

      // actual character should be deleted
      case DELETE:          //aInput[characterIndex] = L' ';
                            // not an empty string and cursor in legal range
                            if(highestCharIndex > 0 && characterIndex < highestCharIndex)
                            {
                                (void) wmemmove(&aInput[characterIndex], &aInput[characterIndex+1], width - (characterIndex + 1));
                                highestCharIndex--;
                            }
                            break;

      // last character should be deleted
      case BACKSPACE:       if(characterIndex > 0)
                            {
                              // deleting last char
                              highestCharIndex--;
                              characterIndex = characterIndex - 1;

                              (void) wmemmove(&aInput[characterIndex], &aInput[characterIndex+1], width - (characterIndex + 1));

                              SetCursorToXY(x + characterIndex, y);
                            }
                            break;

      // return or cursor-down was hit -> quit the input-field with the actual input-string
      case RETURN:
      case CURSOR_DOWN_ID:  quit = TRUE;
                            break;

      // ignore other special keys
      case CURSOR_UP_ID:
      case F1_ID:
      case F2_ID:
      case F3_ID:
      case F4_ID:
      case F5_ID:
      case F6_ID:
      case F7_ID:
      case F8_ID:
      case F9_ID:
      case F10_ID:
      case F11_ID:
      case F12_ID:
                            break;


      // any other key was hit -> evaluate it if it is an numeric key, else just refuse it
      default:              // if input was numeric key, save value in input-array and set cursor to next character if possible

                            if(iswprint(key))
                            {
                              aInput[characterIndex] = key;
                              if(characterIndex < (width-1))
                              {
                                ++characterIndex;
                                if(characterIndex > highestCharIndex)
                                {
                                    highestCharIndex = characterIndex;
                                }
                              }
                            }
                            break;
    }
  }

 // copy rest of input-string
  wcsncpy(pInputString, aInput, width);
  pInputString[highestCharIndex] = '\0';

 return(SUCCESS);
}


int GetInputString(int   x,
                   int   y,
                   int   width,
                   wchar_t* pDefaultString,
                   wchar_t* pInputString)
//
// Get an input-string from user at a specified area on screen. Spaces in front of and at the end of the input-string
// where cut.
//
// input: x, y:           Coordinates where the input-field starts
//        width:          width of input-field, respectivly maximum length of input-string
//        pDefaultString: String with default value for input-string; NULL, if no default-value is given
//        pInputString:   Pointer to a buffer with input-string (must be allocated by the calling function!)
//
{
  int   quit                    = FALSE;
  int   characterIndex          = 0;
  int   key                     = 0;
  wchar_t aInput[LINE_LENGTH + 1] = L"";

  if((width + x) > LINE_LENGTH)
  {
    return(INVALID_PARAMETER);
  }

  // initialize the buffer for the single input-characters, first copy the default-string, if it is given
  if(pDefaultString != NULL)
  {
    swprintf(aInput, LINE_LENGTH, L"%ls", pDefaultString);
  }

  // then fill the rest characters with spaces (if necessairy) and add end-of-string
  if(wcslen(aInput) < width)
  {
    swprintf(&aInput[wcslen(aInput)], LINE_LENGTH + 1, L"%*ls", width - wcslen(aInput), L" ");
  }
  aInput[width] = 0;

  // evaluate the selected keys and initiate the according changes until the user wants to quit
  quit = FALSE;
  while(quit == FALSE)
  {
    // show actual character-buffer on screen
    SetCursorToXY(x, y);
    printf("%ls", aInput);
    SetCursorToXY(x + characterIndex, y);
    fflush(stdout);

    key = Getkey();
    switch(key)
    {
      // cursor should be moved one step left (if it is not already in first position)
      case CURSOR_LEFT_ID:  if(characterIndex > 0)
                            {
                              --characterIndex;
                            }
                            break;

      // cursor should be moved one step right (if it is not already in last position)
      case CURSOR_RIGHT_ID: if(characterIndex < (width - 1))
                            {
                              ++characterIndex;
                            }
                            break;

      // cursor should be moved to first or last position
      case POS_1_ID:        characterIndex = 0;
                            break;
      case END_ID:          characterIndex = width - 1;
                            break;

      // actual character should be deleted
      case DELETE:          aInput[characterIndex] = L' ';
                            break;

      // last character should be deleted
      case BACKSPACE:       if(characterIndex > 0)
                            {
                              characterIndex = characterIndex - 1;
                              aInput[characterIndex] = L' ';
                              SetCursorToXY(x + characterIndex, y);
                            }
                            break;

      // return or cursor-down was hit -> quit the input-field with the actual input-string
      case RETURN:
      case CURSOR_DOWN_ID:  quit = TRUE;
                            break;

      // ignore other special keys
      case CURSOR_UP_ID:
      case F1_ID:
      case F2_ID:
      case F3_ID:
      case F4_ID:
      case F5_ID:
      case F6_ID:
      case F7_ID:
      case F8_ID:
      case F9_ID:
      case F10_ID:
      case F11_ID:
      case F12_ID:
                            break;


      // any other key was hit -> evaluate it if it is an numeric key, else just refuse it
      default:              // if input was numeric key, save value in input-array and set cursor to next character if possible

                            if(iswprint(key))
                            {
                              aInput[characterIndex] = key;
                              if(characterIndex < (width - 1))
                              {
                                ++characterIndex;
                              }
                            }
                            break;
    }
  }

  // jump over the spaces in front of character-buffer
  characterIndex = 0;
  while(aInput[characterIndex] == L' ')
  {
      ++characterIndex;
  }

  // copy rest of input-string
  wcsncpy(pInputString, &aInput[characterIndex], width - characterIndex);

  // get the index of the last character but space in input-string to cut the spaces at the end
  characterIndex = wcslen(pInputString) - 1;
  while((pInputString[characterIndex] == L' ') && (characterIndex > 0))
  {
    --characterIndex;
  }

  // write end-of-string directly behind the last character
  pInputString[characterIndex + 1] = 0;

  return(SUCCESS);
}

int PwdInputBox(int lineCount,
                char **ppLineStrings)
{
    return __InputBox(lineCount,ppLineStrings, true);
}


int InputBox(int     lineCount,
             char**  ppLineStrings)
{
    return __InputBox(lineCount,ppLineStrings, false);
}

int __InputBox(int     lineCount,
             char**  ppLineStrings,
             bool    bPasswordMode)
{
  int   status                        = SUCCESS;
  int   width                         = 0;
  int   quit                          = FALSE;
  int   okSelectLineNr                = 12;
  wchar_t  inputString[LINE_LENGTH + 1]      = L"";
  wchar_t  lastInputString[LINE_LENGTH + 1]  = L"";
  char* defaultString                 = NULL; // char* because part of argv

  teSelectedField eSelectedField    = INPUT;

  // check input-parameter
  if(lineCount < 4)
  {
    return(MISSING_PARAMETER);
  }

  // read string-width and make sure, that input field not exeeds the maximum line-lentgh (width + border-chars + spaces)
  sscanf(ppLineStrings[3], "%d", &width);
  if((width < 1) || (width > (LINE_LENGTH - 4)))
  {
    return(INVALID_PARAMETER);
  }

  // get the default-value if it is given
  if(lineCount > 4)
  {
    defaultString = ppLineStrings[4];
    swprintf(lastInputString, LINE_LENGTH, L"%hs", defaultString);
  }

  // show input-box on screen with title, headline, border of input-field itself, footer and necessairy blank lines
  Clear(0, NULL);
  PrintTitle(ppLineStrings[0]);
  PrintHeadline(ppLineStrings[1]);
  PrintTextLine("");
  PrintTextLine(ppLineStrings[2]);
  PrintBox(width);
  PrintTextLine("");
  PrintTextLine("");
  PrintTextLine("");
  PrintFooter("OK: confirm value, Abort: quit without changes");
  SAVE_CURSOR_POS;

  // evaluate the input from user and initiate the according actions until he wants to quit
  quit = FALSE;
  while(quit == FALSE)
  {
    // cursor should be located in input field
    if(eSelectedField == INPUT)
    {
      // show ok/abort-line with now value selected (no value showed invers)
      SetCursorToXY(1, okSelectLineNr);
      PrintTextLine("< OK >    <Abort>");

      // switch to input-field and get input-value
      if(bPasswordMode)
      {
          GetPasswordString(3, 9, width, lastInputString, inputString);
      }
      else
      {
          GetInputString(3, 9, width, lastInputString, inputString);
      }

      wcsncpy(lastInputString, inputString, LINE_LENGTH);
      lastInputString[LINE_LENGTH] = 0;

      eSelectedField = OK;
    }

    // cursor should be located at ok- or abort-button
    else
    {
      int   key                   = 0;
      char  okString[LINE_LENGTH] = "";

      // show ok/abort-line with the actual selected value showed invers
      SetCursorToXY(1, okSelectLineNr);
      if(eSelectedField == OK)
      {
        sprintf(okString, "%s< OK >%s    <Abort>", INVERS, NORMAL);
      }
      else
      {
        sprintf(okString, "< OK >    %s<Abort>%s", INVERS, NORMAL);
      }
      PrintTextLine(okString);

      // get hit from keyboard (meanwhile the cursor is positioned behind the whole menu)
      RESTORE_CURSOR_POS;
      key = Getkey();
      switch(key)
      {
        // cursor-right, cursor-left, cursor-up: memorize a switch between "ok", "abort" and the input-field to show it in next loop-cycle
        case CURSOR_RIGHT_ID: eSelectedField = ABORT;
                              break;

        case CURSOR_LEFT_ID:  eSelectedField = OK;
                              break;

        case CURSOR_UP_ID:    eSelectedField = INPUT;
                              break;

        // a button was selected with RETURN -> quit window and if the button was "ok", write string to stderr for the calling programm
        case RETURN:          if(eSelectedField == OK)
                              {
                                //printf(":%s:\n", inputString);
                                fprintf(stderr, "%ls", inputString);
                              }
                              else if(eSelectedField == ABORT)
                              {
                                status = ABORT_SELECTED;
                              }

                              quit = TRUE;
                              break;

        // "q" was hit -> quit window without return-value
        case L'q':
        case L'Q':            status = ABORT_SELECTED;
                              quit = TRUE;
                              break;
      }
    }
  }

  return status;
}


int main(int argc, char** argv)
{
  tWindowJumptab *pWindowJumptabEntry = &astWindowJumptab[0];
  int             returnValue         = SUCCESS;
  int             status              = SUCCESS;

  setlocale(LC_ALL, "");

  if(argc <= 1)
  {
    returnValue = MISSING_PARAMETER;
  }
  else
  {
    while(pWindowJumptabEntry->pcFunctionName != NULL)
    {
      if(strcmp(pWindowJumptabEntry->pcFunctionName, argv[1]) == 0)
      {
        returnValue = pWindowJumptabEntry->pfFunction(argc - 2, &argv[2]);
        break;
      }

     ++pWindowJumptabEntry;
    }

    // function not found?
    if(pWindowJumptabEntry->pcFunctionName == NULL)
    {
      returnValue = MISSING_PARAMETER;
    }
  }

  if(returnValue < 0)
  {
    ShowErrorText();
  }

  //printf("returnValue:%d", returnValue);
  return(returnValue);
}
