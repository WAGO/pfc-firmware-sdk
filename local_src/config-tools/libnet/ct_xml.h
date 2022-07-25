// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef __CT_XML_H__
#define __CT_XML_H__

#include <glib.h>
#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct xmlSession xmlSession_t;

int ct_xml_generate_network_config(const char *outFilename, xmlSession_t *handle);
int ct_xml_commit_changes(xmlSession_t *handle);
int ct_xml_commit_xml_changes(xmlSession_t *handle);

int ct_xml_start_session(const char *xmlConfigFile, xmlSession_t **handle);
int ct_xml_finish_session(xmlSession_t *handle);

void ct_xml_permanent_close(void);

int ct_xml_has_key(const char *xpathExpr, bool * const fHasKey, xmlSession_t *sessionHandle);

int ct_xml_set_value(const char * xpathExpr, const char * value, xmlSession_t *handle);
int ct_xml_add_value(const char *xpathExpr, const char *value, xmlSession_t *sessionHandle);
int ct_xml_get_value(const char *xpathExpr, char *result, size_t resultLen, xmlSession_t *handle);
int ct_xml_dup_value(const char *xpathExpr, char **result, xmlSession_t *sessionHandle);

int ct_xml_get_multiple_values(const char *xpathExpr, GString *result, const char *delim, xmlSession_t *handle);
int ct_xml_set_multiple_values(const char *xpathExpr, const char *value, xmlSession_t *handle);
int ct_xml_del_multiple_values(const char *xpathExpr, xmlSession_t *sessionHandle);

__END_DECLS

#endif
