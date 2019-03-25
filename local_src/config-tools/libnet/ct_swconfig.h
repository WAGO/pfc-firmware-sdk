// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef __CT_SWCONFIG_H__
#define __CT_SWCONFIG_H__

#include <glib.h>

typedef struct swconfigSession swconfigSession_t;

int ct_swconfig_start_session(const char *switchDev, swconfigSession_t **handle);
void ct_swconfig_finish_session(swconfigSession_t *handle);

int ct_swconfig_set_attr(const char *attrName, const char *attrValue, int portNr, swconfigSession_t *sessionHandle);
int ct_swconfig_get_attr(const char *attrName, char *attrValue, size_t attrValueLen, swconfigSession_t *sessionHandle);

#endif
