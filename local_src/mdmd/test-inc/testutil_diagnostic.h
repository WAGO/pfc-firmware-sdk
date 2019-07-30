// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef TESTUTIL_DIAGNOSTIC_H
#define TESTUTIL_DIAGNOSTIC_H

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

  #include "diagnostic/diagnostic_API.h"

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    
void        test_log_EVENT_init();
bool        test_log_EVENT_InitDone();
const char* test_get_log_EVENT_ProgramName();
bool        test_log_EVENT_LogId_get_first(log_tEventId *id, bool *set);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#endif /* TESTUTIL_DIAGNOSTIC_H */
//---- End of source file ------------------------------------------------------

