// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef __CT_LIBLOG_H__
#define __CT_LIBLOG_H__

#include <ct_error_handling.h>

int ct_liblog_setLastError(const char *errorStr);

// \Note: This function is deprecated an will be removed soon.
//        Do not use!
int ct_liblog_reportError(enum eStatusCode errNum, const char *additionalStr) __attribute__((deprecated("will be removed soon")));

#endif
