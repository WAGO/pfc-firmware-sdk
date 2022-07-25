//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     config_linux_user.c
///
///  \version  $Revision: 65689 $1
///
///  \brief    
///
///  \author   Hans-Florian Scholz : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <crypt.h>
#include <shadow.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>

#include "config_tool_lib.h"

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


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

static void get_random_bytes(unsigned char * buf, size_t szBuf)
{
  int fd;
  int rbytes=0;
  fd = open("/dev/random", O_NONBLOCK | O_RDONLY);
  rbytes = read(fd,buf,szBuf);
  close(fd);
  //if random dis not get enough ytes use urandom
  if(rbytes < szBuf)
  {
    fd = open("/dev/urandom",  O_RDONLY);
    read(fd,buf+rbytes,szBuf-rbytes);
    close(fd);
  }
}

static char * CryptPwd(char *pwd)
{
  char * newHash = NULL;
  int i;
  unsigned char NaCl[8];//natrium Clorid
  char salt[] = "$1$........";
  const char NaClchars[] =
      "./0123456789ABCDEFGHIJKLMNOPQRST"
      "UVWXYZabcdefghijklmnopqrstuvwxyz";

  //create random bytes for making random salt
  get_random_bytes(NaCl, sizeof(NaCl));

  //make salt
  for (i = 0; i < 8; i++)
  {
    salt[3+i] = NaClchars[NaCl[i] % (sizeof(NaClchars)-1)];
  }

  //encrypt password
  newHash = crypt(pwd, salt);

  return newHash;
}

static int FillUserData( struct spwd  * newUserData, char *username, char *np1, char *np2)
{
  //user=admin new-password=abcd confirm-password=abcd

  char *np1Unescaped;

  if(!strcmp(newUserData->sp_namp,"root"))
  {
    return INVALID_PARAMETER;
  }
  if((np1 == NULL || np2 == NULL) || strcmp(np1,np2))
  {
    return INVALID_PARAMETER;
  }

  // Assumption: special characters are URI-encoded (i.e. '!' => %21 etc.)
  np1Unescaped = g_uri_unescape_string(np1, "");

  if(NULL == np1Unescaped)
  {
    return INVALID_PARAMETER;
  }
 
  newUserData->sp_pwdp = CryptPwd(np1Unescaped);
  newUserData->sp_lstchg = time(NULL) / (86400);

  // glib unescape-functions use dynamic allocation
  free(np1Unescaped);

  return SUCCESS;
}

static int FillUserDataFromHash( struct spwd  * newUserData, char *username, char *hash)
{
  char *userEntry = NULL;
  struct spwd *_newUserData = NULL;
  int status = ERROR;

  if(!strcmp(username,"root"))
  {
    return INVALID_PARAMETER;
  }

  status = SYSTEM_CALL_ERROR;
  userEntry = malloc(strlen(username) + strlen(hash) + 2);

  if(NULL != userEntry)
  {
    strcpy(userEntry, username);
    strcat(userEntry, ":");
    strcat(userEntry, hash);

    if(NULL != (_newUserData = sgetspent(userEntry)))
    {
      memcpy(newUserData, _newUserData, sizeof(struct spwd));
//      free(_newUserData); // sgetspent seems to return no malloc'ed pointer => free causes SEGFAULT
      status = SUCCESS;
    }

    free(userEntry);
  }

  return status;
}

static int WriteShadow(struct spwd  * newUserData)
{
  FILE * fpShadowOld = NULL;
  FILE * fpShadowNew = NULL;
  const char * newShadowPath = _PATH_SHADOW"-";
  struct spwd *spwd = NULL;
  int changed = 0;

  if(0 != lckpwdf())
  {
      return FILE_OPEN_ERROR;
  }
  fpShadowOld = fopen(_PATH_SHADOW,"r");
  fpShadowNew = fopen(newShadowPath,"w");
  if(fpShadowOld == NULL)
  {
    return 0;
  }
  if(fpShadowNew == NULL)
  {
    return 0;
  }
  spwd = fgetspent(fpShadowOld);
  changed = 0;
  while(spwd != NULL)
  {
    if(!strcmp(newUserData->sp_namp,spwd->sp_namp))
    {
      spwd->sp_pwdp = newUserData->sp_pwdp;
      spwd->sp_lstchg = newUserData->sp_lstchg;
      changed = 1;
    }
    putspent(spwd,fpShadowNew);
    spwd = fgetspent(fpShadowOld);
  }
  fclose(fpShadowOld);
  fclose(fpShadowNew);
  if(changed)
  {
    rename(newShadowPath,_PATH_SHADOW);
  }
  else
  {
    unlink(newShadowPath);
    return NOT_FOUND;
  }
  ulckpwdf();

  return SUCCESS;
}

void PrintHelp(void)
{
    printf("\n* Change the passwords of the linux-user *\n\n");
    printf("Usage: config_linux_user < user=user-value new-password=new-password-value confirm-password=confirm-password-value >\n\n");
    printf("       config_linux_user < user=user-value hash=new-hash-value");
    printf("user-value: name of the webserver-user, whose password should be changed\n"
           "new-password-value / confirm-password-value: both strings must be the same and URI encoded\n"
           "hash: password hash and additional infos in /etc/shadow format\n");
}

int main(int    argc, 
         char** argv)
{
  int   status     = SUCCESS;

  char *username = NULL;
  char *password1 = NULL;
  char *password2 = NULL;
  char *hash = NULL;

  enum pwdModes {INVALID=-1,PLAIN,HASH};

  enum pwdModes pwdMode = INVALID;

  struct spwd newUserData;
  memset(&newUserData, 0, sizeof(struct spwd));
  
  do
  {
    int nr = 0;

    if(argc < 2)
    {
      status = MISSING_PARAMETER;
      break;
    }

    // Parse arguments ( no dashes -> no getopt :( )
    if(argc < 2 || 
       0 == strcmp(argv[1], "-h") ||
       0 == strcmp(argv[1], "--help"))
    {
      PrintHelp();
      break;
    }

    for(nr=1; nr<argc; ++nr)
    {
      if(!strncmp(argv[nr],"user=",strlen("user=")))
      {
        username = argv[nr] + strlen("user=");
      }
      else if(!strncmp(argv[nr],"new-password=",strlen("new-password=")))
      {
        password1 = argv[nr] + strlen("new-password=");
        pwdMode = PLAIN; 
      }
      else if(!strncmp(argv[nr],"confirm-password=",strlen("confirm-password=")))
      {
        password2 = argv[nr] + strlen("confirm-password=");
        pwdMode = PLAIN;
      }
      else if(!strncmp(argv[nr],"hash=",strlen("hash=")))
      {
        hash = argv[nr] + strlen("hash=");
        pwdMode = HASH; 
      }
      else
      {
        PrintHelp();
        status = INVALID_PARAMETER;
        break;
      }
    }

    switch(pwdMode)
    {
    case PLAIN:
      if(NULL == hash && 
         4 == argc) 
      {
        newUserData.sp_namp = strdup(username); // do it in the same nesting level as free 
                                                // (see cleanup section below)
        status = FillUserData(&newUserData,username, password1, password2); // allocates sp_pwdp via crypt()
      }
      else
      {
        status = (4 == argc? INVALID_PARAMETER : MISSING_PARAMETER);
      }

      break;

    case HASH:
      if(NULL == password1 &&
         NULL == password2 &&
         3 == argc)
      {
        status = FillUserDataFromHash(&newUserData, username, hash);
      }
      else
      {
        status = (3 == argc? INVALID_PARAMETER : MISSING_PARAMETER);
      }

      break;

    default:
      status = INVALID_PARAMETER;
      break;
    }

    if(SUCCESS != status)
    {
      break;
    }

    if ((getspnam(newUserData.sp_namp)) == NULL)
    {
      status = NOT_FOUND;
      break;
    }

    if((status = WriteShadow(&newUserData))!= SUCCESS)
    {
      break;
    }
 
    sync(); 

  } while(0);

  // Cleanup 
  // 
  // Note: 'man shadow' is very vague on its internal memory management
  // details. It is unclear whether the user has to clean up struct spwd
  // objects.  
  // At least valgrind shows no memory leaks when no cleanup is done there.
  // That's why we only clean up self-allocated strings we use with struct spwd
  // objects. 
  
  if(PLAIN == pwdMode)
  {
    free(newUserData.sp_namp);
    free(newUserData.sp_pwdp);
  }

  return status;

}
