// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef __CT_XSLT_H__
#define __CT_XSLT_H__

#include <libxml/tree.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

int ct_xslt_apply_transformation(xmlDocPtr doc, const char *stylesheetName, int resFd);

__END_DECLS

#endif
