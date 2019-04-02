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
///  \file     liboms_internal.h
///
///  \version  $Revision: 1 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LIBOMS_INTERNAL_H_
#define LIBOMS_INTERNAL_H_

#include <wago_oms_API.h>

extern struct timeval eventTime;

int SM_UpdateStateMaschine(tOmsDevice * dev);


#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define PROJECT_DIR_NAME "oms-0.1"
//#define PDN_LENGTH       sizeof(PROJECT_DIR_NAME)
#define PDN_LENGTH       0
#define DBG2( fmt, ...) \
  do { printf( "%s:%d:%s(): "fmt"\n",\
               strstr(__FILE__,PROJECT_DIR_NAME)+PDN_LENGTH,__LINE__,__func__, \
               __VA_ARGS__); } while (0)
#define DBG(fmt) DBG2("%s",fmt)
#define DBGFUNC() printf("%s()\n",__func__)
#define DBGINITERR() (errno=0)
#define DBGERR(fmt) printf("%s().%s:%s\n",__func__,fmt,strerror(errno))
#else
#define DBG2( fmt, ...)
#define DBG(fmt)
#define DBGFUNC()
#define DBGINITERR()
#define DBGERR(x)
#endif

#endif /* LIBOMS_INTERNAL_H_ */
//---- End of source file ------------------------------------------------------
