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
///  \file     common.h
///
///  \version  $Revision: 12545 $
///
///  \brief    <Insert description here>
///
///  \author   HFS : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef COMMON_H_
#define COMMON_H_
#include "config.h"

#define UNUSED_PARAMETER(x) ((void)x)
#ifdef DEBUG
#include <stdio.h>
#if 1
#define DBG2( fmt, ...) \
  do { printf(" %s:%d:%s(): "fmt"\n",\
               __FILE__,__LINE__,__func__, \
               __VA_ARGS__); } while (0)
#define DBG(fmt) DBG2("%s",fmt)
#define DBGFUNC() printf("%s()\n",__func__)
#else
#include <syslog.h>
#define DBG2( fmt, ...) \
  do { syslog(LOG_DEBUG, " %s:%d:%s(): "fmt"\n",\
               __FILE__,__LINE__,__func__, \
               __VA_ARGS__); } while (0)
#define DBG(fmt) DBG2("%s",fmt)
#define DBGFUNC() syslog(LOG_DEBUG,"%s()\n",__func__)
#endif
#else
#define DBG2(...)
#define DBG(...)
#define DBGFUNC()
#endif

#endif /* COMMON_H_ */
//---- End of source file ------------------------------------------------------
