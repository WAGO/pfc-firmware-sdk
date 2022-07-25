// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <string.h>
#include <assert.h>

#include <libxml/xpath.h>

#include "ct_xml_xpath.h"
#include "../ct_error_handling.h"

typedef struct xpathSession {
    xmlXPathContextPtr xpathContext;
    xmlNode *rootNode;
    xmlDoc *doc;
} xpathSession_t;

static int __xpath_eval(xpathSession_t *session, const xmlChar * xpathExpr, xmlXPathObjectPtr *result)
{
    int status = SUCCESS;

    assert(NULL != session);
    assert(NULL != result);

    *result = xmlXPathEvalExpression((xmlChar*) xpathExpr, session->xpathContext);

    if(NULL == *result)
    {
        status = INVALID_PARAMETER;
    }
    else if (xmlXPathNodeSetIsEmpty((*result)->nodesetval))
    {
        xmlXPathFreeObject(*result);
        *result = NULL;
        status = NOT_FOUND;
    }
    return status;
}

int ct_xml_xpath_start_session(xmlDocPtr doc, xpathSession_t **session)
{
    int status = SUCCESS;
    xpathSession_t *sp;

    assert(NULL != session);
    assert(NULL != doc);

    sp = g_malloc(sizeof(xpathSession_t));
    sp->doc = doc;
    sp->xpathContext = xmlXPathNewContext(doc);
    if(NULL == sp->xpathContext)
    {
        status = FILE_READ_ERROR;
    }
    if(SUCCESS == status)
    {
        sp->xpathContext->node = xmlDocGetRootElement(doc);
    }
    else
    {
        g_free(sp);
        sp = NULL;
    }
    *session = sp;
    return status;
}

void ct_xml_xpath_finish_session(xpathSession_t *session)
{
    assert(NULL != session);
    xmlXPathFreeContext(session->xpathContext);
    g_free(session);
}


int ct_xml_xpath_has_key(xpathSession_t *session, const xmlChar * xpathExpr, bool * const fHasKey)
{
    xmlXPathObjectPtr xpathResult = NULL;
    int status = __xpath_eval(session, xpathExpr, &xpathResult);

    *fHasKey = (   (SUCCESS == status)
                   && (NULL != xpathResult));

    if (   (NOT_FOUND == status)
        || (INVALID_PARAMETER == status))
    {
        status = SUCCESS;
    }

    return status;
}


int ct_xml_xpath_get_value(xpathSession_t *session, const xmlChar * xpathExpr, xmlChar *result, size_t resultLen)
{
    int status = SUCCESS;

    assert(NULL != session);
    assert(NULL != result);
    assert(0 == strlen((char*) result));

    xmlXPathObjectPtr xpathResult = NULL;

    status = __xpath_eval(session, xpathExpr, &xpathResult); 

    if(SUCCESS == status)
    {
        xmlChar *resStr = xmlNodeListGetString(session->doc, xpathResult->nodesetval->nodeTab[0]->children, 1); 

        if(NULL == resStr) // empty node, i.e. <ip/>
        {
            status = NOT_FOUND;
        }
        else
        {
            (void) g_strlcpy((char *) result, (char*) resStr, resultLen);

            if(xmlStrlen(result) < xmlStrlen(resStr))
            {
                status = NOT_ENOUGH_SPACE_ERROR;
            }

            xmlFree(resStr);
        }

        xmlXPathFreeObject(xpathResult);
    }

    return status;
}

// Returns g_strdup() of requested value in result. Needs to be freed with g_free().
int ct_xml_xpath_dup_value(xpathSession_t *session, const xmlChar *xpathExpr, xmlChar **result)
{
    int status = SUCCESS;

    assert(NULL != session);
    assert(NULL != result);

    xmlXPathObjectPtr xpathResult = NULL;

    status = __xpath_eval(session, xpathExpr, &xpathResult); 

    if(SUCCESS == status)
    {
        xmlChar *resStr = xmlNodeListGetString(session->doc, xpathResult->nodesetval->nodeTab[0]->children, 1); 

        if(NULL == resStr) // empty node, i.e. <ip/>
        {
            status = NOT_FOUND;
        }
        else
        {
            *result =  (xmlChar *)g_strdup((char *)resStr);
        }

        xmlXPathFreeObject(xpathResult);
    }

    return status;
}

int ct_xml_xpath_set_multiple_values(xpathSession_t *session, const xmlChar *xpathExpr, const char *value)
{
    int status = SUCCESS;
 
    assert(NULL != session);
    assert(NULL != xpathExpr);
    assert(NULL != value);
     
    xmlXPathObjectPtr xpathResult = NULL;

    status = __xpath_eval(session, xpathExpr, &xpathResult);
 
    if(SUCCESS == status)
    {

        xmlNodeSetPtr nodes = xpathResult->nodesetval;    

        int i;
        // Reversed order: see comment at http://www.xmlsoft.org/examples/xpath2.c
        for(i = nodes->nodeNr - 1; i>=0; --i)
        {
            assert(nodes->nodeTab[i]);
            xmlNodeSetContent(nodes->nodeTab[i], (const xmlChar*) value);

            // Removing elements from nodeset: see comment at http://www.xmlsoft.org/examples/xpath2.c
            if(nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            {
                nodes->nodeTab[i] = NULL;
            }
        }
    }
 
    return status;
}

// Removes all nodes matching the XPath.
int ct_xml_xpath_del_multiple_values(xpathSession_t *session, const xmlChar *xpathExpr)
{
    int status = SUCCESS;
 
    assert(NULL != session);
    assert(NULL != xpathExpr);
     
    xmlXPathObjectPtr xpathResult = NULL;

    status = __xpath_eval(session, xpathExpr, &xpathResult);
 
    if(SUCCESS == status)
    {

        xmlNodeSetPtr nodes = xpathResult->nodesetval;    

        // Reversed order: see comment at http://www.xmlsoft.org/examples/xpath2.c
        int i;
        for(i = nodes->nodeNr - 1; i>=0; --i)
        {
            assert(nodes->nodeTab[i]);
            xmlUnlinkNode(nodes->nodeTab[i]);
            xmlFreeNode(nodes->nodeTab[i]);

            // Removing elements from nodeset: see comment at http://www.xmlsoft.org/examples/xpath2.c
            if(nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            {
                nodes->nodeTab[i] = NULL;
            }
        }
    }
    else if(NOT_FOUND == status)
    {
        // No node found. Done.
        status = SUCCESS;
    }

    return status;
}

int ct_xml_xpath_get_multiple_values(xpathSession_t *session, const xmlChar *xpathExpr, GString *result, const char *delim)
{
    int status = SUCCESS;

    assert(NULL != session);
    assert(NULL != xpathExpr);
    assert(NULL != result);
    assert(0 == result->len);

    xmlXPathObjectPtr xpathResult = NULL;

    status = __xpath_eval(session, xpathExpr, &xpathResult); 

    if(SUCCESS == status)
    {
        int subres = 0;
        const char *_delim = "";

        for(subres = 0; subres < xpathResult->nodesetval->nodeNr; ++subres)
        {
            g_string_append_printf(result,
                                   "%s%s", _delim,
                                   (char*) xmlNodeListGetString(session->doc, 
                                   xpathResult->nodesetval->nodeTab[subres]->children, 1));
            _delim = delim;
        }

        if(0 == result->len)
        {
            status = NOT_FOUND;
        }

        xmlXPathFreeObject(xpathResult);
    }

    return status;

}

int ct_xml_xpath_add_value(xpathSession_t *session, const xmlChar *xpathExpr, const xmlChar *newVal)
{
    int status = SUCCESS;

    assert(NULL != session);
    assert(NULL != newVal);

    xmlXPathObjectPtr xpathResult = NULL;

    char *last_slash = g_strrstr((const gchar *)xpathExpr, "/");
    assert(NULL != last_slash);
    char *xpathparent = g_strndup((const gchar *)xpathExpr, (int)(last_slash - (char *)xpathExpr));
    last_slash++;
    assert('\0' != *last_slash);

    status = __xpath_eval(session, (const xmlChar *)xpathparent, &xpathResult);
    if(SUCCESS == status)
    {
        xmlNewTextChild(xpathResult->nodesetval->nodeTab[0], NULL, (const xmlChar *)last_slash, newVal);
        xmlXPathFreeObject(xpathResult);
    }
    g_free(xpathparent);
    return status;
}

int ct_xml_xpath_set_value(xpathSession_t *session, const xmlChar * xpathExpr, const xmlChar *newVal)
{
    int status = SUCCESS;

    assert(NULL != session);
    assert(NULL != newVal);

    xmlXPathObjectPtr xpathResult = NULL;

    status = __xpath_eval(session, xpathExpr, &xpathResult); 

    if(SUCCESS == status)
    {
        xmlNode *text = xpathResult->nodesetval->nodeTab[0]->children;

        if(NULL == text) // empty node, i.e. <ip/> => create text child
        {
            if(NULL == (text = xmlNewText(newVal)))
            {
                status = SYSTEM_CALL_ERROR;
            }
            else
            {
                xmlAddChild(xpathResult->nodesetval->nodeTab[0], text);
            }
        }
        else
        {
            xmlNodeSetContentLen(xpathResult->nodesetval->nodeTab[0]->children,
                                 newVal, xmlStrlen(newVal));
        }

        xmlXPathFreeObject(xpathResult);
    }

    return status;
}

