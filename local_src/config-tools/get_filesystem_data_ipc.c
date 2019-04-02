#if 0

int GetSecondActiveDevice(char* pOutputString,
                          int   additionalParam)
//
// Get the actual active partition from menu.lst
//
// input: ignored
//
// output: string with the active partition
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                                                = SUCCESS;
  char* pMenuLstFileContent                                   = NULL;

  UNUSED_PARAMETER(additionalParam);

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  *pOutputString = '\0';

  // get content of menu.lst-file
  pMenuLstFileContent = FileContent_Get("/boot/grub/menu.lst");

  if(pMenuLstFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  menuLstLine[MAX_LENGTH_OUTPUT_LINE]         = "";
    int   lineNr                                      = 0;

    // loop over the lines of menu.lst-file until we find the line with the line with the root-string
    lineNr = 1;
    while((SUCCESS == SystemCall_GetLineByNr(pMenuLstFileContent, lineNr, menuLstLine)) && (strlen(pOutputString) == 0))
    {   
      // first search for the string which is always directly in front of the firmware-revision
      const char  beginOfDeviceString[] = "root=";
      char*       deviceString          = strstr(menuLstLine, beginOfDeviceString);
      //printf("%s\n", menuLstLine);

      // if the line is no comment and introducing string was found
      if((menuLstLine[0] != COMMENT_CHAR) && (deviceString != NULL))
      {
        // read the following device-string and remove possible line-feed or similar behind it
        strncpy(pOutputString, deviceString + strlen(beginOfDeviceString), MAX_LENGTH_OUTPUT_STRING);
        CutWord(pOutputString, MAX_LENGTH_OUTPUT_STRING);
      }

      ++lineNr;
    }
  }

  FileContent_Destruct(&pMenuLstFileContent);

  return(status);
}

#endif
