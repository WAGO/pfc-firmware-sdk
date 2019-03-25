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
///  \file     get_uimage_size.c
///
///  \version  $Revision: 6731 $
///
///  \brief    This config tool extracts the file size info from u-boot-style
///           uImage header. The size info is needed to copy the kernel file
///           from a NAND/MTD device to the file system.
///
///  \author   <AGa> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <arpa/inet.h>

// USE_HOSTCC is hard-coded in mkimage's makefile
// It is used inside image.h and has thus to be set here
#define USE_HOSTCC

#include <u-boot-mkimage/image.h>

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#ifdef  __CPPUTEST__ 
#define main get_uimage_file_main
#endif

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

void Usage(void)
{
	printf("\nget_uimage_size: read uImage header and print file size as hex value.\n\n"
"Usage:\n"
" get_uimage_size file\n\n"
"\n"
	);
}

int GetUimageSize(const char *filename, int *imgSize)
{
  int ret = EXIT_SUCCESS;
  
  image_header_t imgHdr;
  size_t hdrSize = sizeof(imgHdr);

  FILE *uimg = NULL;

  do
  {

    uimg = fopen(filename, "rb");

    if(NULL == uimg)
    {
      perror("fopen");
      ret = EXIT_FAILURE;
      break;
    }

    int nrRead = fread(&imgHdr, hdrSize, 1, uimg);

    if(1 != nrRead)
    {
      printf("fread: error reading header.\n");
      ret = EXIT_FAILURE;
      break;
    }
 
    if(IH_MAGIC != ntohl(imgHdr.ih_magic))
    {
      printf("%s is no valid uImage file\n", filename);
      ret = EXIT_FAILURE;
      break;
    }
  
    *imgSize = ntohl(imgHdr.ih_size);

  } while(0);

  if(NULL != uimg)
  {
    fclose(uimg);
  }

  return ret;
}

int main(int argc, char **argv)
{
  unsigned int imgSize = -1;
  int ret;

  if(argc != 2)
  {
    Usage();
    return EXIT_FAILURE;
  }
  
  ret = GetUimageSize(argv[1], &imgSize);

  if(EXIT_SUCCESS == ret)
  {
    printf("0x%x\n", imgSize);
  }

  return ret;
}

