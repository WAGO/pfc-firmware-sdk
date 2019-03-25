// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "mdmd_log.h"

/// global Mdmd logging masks
unsigned long _mdmdLogLevel = MD_LOG_DEFAULT;


unsigned int mdmd_GetSyslogLogLevel(unsigned int mdmd_logLevel)
{
   switch (mdmd_logLevel)
   {
      case MD_LOG_ERR:
         return LOG_ERR;
      case MD_LOG_WRN:
         return LOG_WARNING;
      case MD_LOG_INF:
         return LOG_INFO;
      case MD_LOG_DBG:
        /* no break, fall through */
      case MD_LOG_DBG2:
        /* no break, fall through */
      default:
         return LOG_DEBUG;
   }
}

int mdmd_GetLogLevel( void )
{
  int retVal;
  switch (_mdmdLogLevel)
  {
     case MD_LOG_ERR: retVal = MD_LOG_ERR; break;
     case MD_LOG_WRN: retVal = MD_LOG_WRN; break;
     case MD_LOG_INF: retVal = MD_LOG_INF; break;
     case MD_LOG_DBG: retVal = MD_LOG_DBG; break;
     case MD_LOG_DBG2: retVal = MD_LOG_DBG2; break;
     default:         retVal = -1; break; //urgh, this should not happen
  }
  return retVal;
}

bool mdmd_SetLogLevel( int logLevel )
{
  bool retVal= true;
  switch (logLevel)
  {
     case MD_LOG_ERR: _mdmdLogLevel = MD_LOG_ERR; break;
     case MD_LOG_WRN: _mdmdLogLevel = MD_LOG_WRN; break;
     case MD_LOG_INF: _mdmdLogLevel = MD_LOG_INF; break;
     case MD_LOG_DBG: _mdmdLogLevel = MD_LOG_DBG; break;
     case MD_LOG_DBG2: _mdmdLogLevel = MD_LOG_DBG2; break;
     default:         retVal= false; break;
  }
  return retVal;
}

