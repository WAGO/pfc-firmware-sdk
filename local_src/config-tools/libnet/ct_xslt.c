// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "ct_xslt.h"
#include "../ct_error_handling.h" 


int ct_xslt_apply_transformation(xmlDocPtr doc, const char *stylesheetName, int resFd)
{
    int status = SUCCESS;
    const char *params[1];
    xmlDocPtr resXml = NULL;
    xsltStylesheetPtr stylesheet = NULL;

    *params = NULL;
    
    do {

        if(NULL == (stylesheet = xsltParseStylesheetFile((const xmlChar*) stylesheetName)) )
        {
            status = FILE_READ_ERROR;
            break;
        }


        if(NULL == (resXml = xsltApplyStylesheet(stylesheet, doc, params)) )
        {
            status = SYSTEM_CALL_ERROR;
            break;
        }

        if(-1 == xsltSaveResultToFd(resFd, resXml, stylesheet) )
        {
            status = SYSTEM_CALL_ERROR;
            break;
        }

    } while(0);

    if(stylesheet)
    {
        xsltFreeStylesheet(stylesheet);
    }

    xsltCleanupGlobals();    

    return status;
}
