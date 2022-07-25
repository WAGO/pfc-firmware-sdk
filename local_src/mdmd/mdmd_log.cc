// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "mdmd_log.h"

/// global Mdmd logging masks
unsigned long _mdmdLogLevel = MD_LOG_DEFAULT;

bool mdmd_SetLogLevel( int logLevel )
{
  bool retVal= true;
  switch (logLevel)
  {
     case MD_LOG_OFF: _mdmdLogLevel = MD_LOG_OFF; break;
     case MD_LOG_ERR: _mdmdLogLevel = MD_LOG_ERR; break;
     case MD_LOG_WRN: _mdmdLogLevel = MD_LOG_WRN; break;
     case MD_LOG_INF: _mdmdLogLevel = MD_LOG_INF; break;
     case MD_LOG_DBG: _mdmdLogLevel = MD_LOG_DBG; break;
     case MD_LOG_DBG2: _mdmdLogLevel = MD_LOG_DBG2; break;
     default:         retVal= false; break;
  }
  return retVal;
}

