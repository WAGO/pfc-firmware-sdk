// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <net/if.h>

#include <glib.h>
#include <libxml/tree.h>

#include "ct_xml.h"
#include "ct_xml_node_handlers.h"
#include "ct_xml_xpath.h"
#include "../ct_error_handling.h"
#include "ct_xslt.h"


// TODO: protect w/interprocess mutex
struct xmlSession
{
    xmlDocPtr   xmlPtr; 
    char *xmlFilename;
    xpathSession_t *xpathSession;
};

static int __getAdvisoryFileLock(const char *filename)
{
    int status = SUCCESS;

    int fd = open(filename, O_RDONLY);

    if(-1 == fd)
    {
        status = FILE_OPEN_ERROR;
    }

    if(SUCCESS == status)
    {
        if(-1 == flock(fd, LOCK_EX))
        {
            status = FILE_OPEN_ERROR;
        }

    }

    if(-1 != fd)
    {
        close(fd);
    }

    return status;
}

static int __releaseAdvisoryFileLock(const char *filename)
{

    int status = SUCCESS;

    int fd = open(filename, O_RDONLY);

    if(-1 == fd)
    {
        status = FILE_OPEN_ERROR;
    }

    if(SUCCESS == status)
    {
        if(-1 == flock(fd, LOCK_UN))
        {
            status = FILE_OPEN_ERROR;
        }

    }

    if(-1 != fd)
    {
        close(fd);
    }

    return status;
}

int ct_xml_start_session(const char *xmlConfigFile, xmlSession_t **sessionHandle)
{
    int status = SUCCESS;

    assert(NULL != sessionHandle);
    assert(NULL == *sessionHandle);

    status = __getAdvisoryFileLock(xmlConfigFile);

    *sessionHandle = g_malloc(sizeof(xmlSession_t));

    xmlSession_t *ressources = *sessionHandle;

    if(NULL == ressources)
    {
        status = SYSTEM_CALL_ERROR;
    }
    else
    {
        ressources->xmlFilename = g_strdup(xmlConfigFile);
    
        LIBXML_TEST_VERSION
        
        xmlInitParser();

        ressources->xmlPtr = xmlParseFile(xmlConfigFile);

        if(NULL == ressources->xmlPtr)
        {
            status = FILE_OPEN_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        status = ct_xml_xpath_start_session(ressources->xmlPtr, &ressources->xpathSession);
    }

    if(SUCCESS != status)
    {
        (void) __releaseAdvisoryFileLock(xmlConfigFile);

        if(NULL != *sessionHandle)
        {
            g_free(*sessionHandle);
            *sessionHandle = NULL;
        }
    }

    return status;
}

int ct_xml_finish_session(xmlSession_t *sessionHandle)
{

    int status = SUCCESS;

    if(NULL != sessionHandle && NULL != sessionHandle->xmlPtr)
    {
        ct_xml_xpath_finish_session(sessionHandle->xpathSession);

        // Lock is released on process exit anyway
        status = __releaseAdvisoryFileLock(sessionHandle->xmlFilename);
        
        xmlFreeDoc(sessionHandle->xmlPtr);
        sessionHandle->xmlPtr = NULL;

        g_free(sessionHandle->xmlFilename);

        g_free(sessionHandle);

    }

    return status;
}

void ct_xml_permanent_close(void)
{
    // xmlCleanupParser frees some global memory that cannot be expicitely
    // reclaimed, causing SEGFAULTs on subsequent libxml2 function
    // calls!
    xmlCleanupParser();
}

int ct_xml_has_key(const char *xpathExpr, bool * const fHasKey, xmlSession_t *sessionHandle)
{
    return ct_xml_xpath_has_key(sessionHandle->xpathSession, (const xmlChar*) xpathExpr, fHasKey);
}

int ct_xml_set_value(const char *xpathExpr, const char *value, xmlSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);

    return ct_xml_xpath_set_value(sessionHandle->xpathSession, (const xmlChar*) xpathExpr, (const xmlChar*) value);
}

int ct_xml_get_value(const char *xpathExpr, char *result, size_t resultLen, xmlSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);
    assert(NULL != xpathExpr);
    assert(NULL != result);
    assert(0 == strlen(result));
    
    return ct_xml_xpath_get_value(sessionHandle->xpathSession, (const xmlChar*) xpathExpr, (xmlChar*) result, resultLen);
}

int ct_xml_dup_value(const char *xpathExpr, char **result, xmlSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);
    assert(NULL != xpathExpr);
    assert(NULL != result);

    return ct_xml_xpath_dup_value(sessionHandle->xpathSession, (const xmlChar *)xpathExpr, (xmlChar **)result);
}

int ct_xml_set_multiple_values(const char *xpathExpr, const char *value, xmlSession_t *sessionHandle)
{
    assert(NULL != xpathExpr);
    assert(NULL != value);
    assert(NULL != sessionHandle);

    return ct_xml_xpath_set_multiple_values(sessionHandle->xpathSession, (const xmlChar*) xpathExpr, value);
}

int ct_xml_del_multiple_values(const char *xpathExpr, xmlSession_t *sessionHandle)
{
    assert(NULL != xpathExpr);
    assert(NULL != sessionHandle);

    return ct_xml_xpath_del_multiple_values(sessionHandle->xpathSession, (const xmlChar*) xpathExpr);
}

int ct_xml_get_multiple_values(const char *xpathExpr, GString *result, const char *delim, xmlSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);

    return ct_xml_xpath_get_multiple_values(sessionHandle->xpathSession, (const xmlChar*) xpathExpr, result, delim);
}

int ct_xml_add_value(const char *xpathExpr, const char *value, xmlSession_t *sessionHandle)
{
    assert(NULL != xpathExpr);
    assert(NULL != value);
    assert(NULL != sessionHandle);

    return ct_xml_xpath_add_value(sessionHandle->xpathSession, (const xmlChar*)xpathExpr, (const xmlChar *)value);
}

enum relSaveToDiskModes
{
    REL_SAVE_INTERFACES_XML,
    REL_SAVE_ETC_NET_IFACES,
};

#define NETWORK_INTERFACES_XSLT "/etc/specific/network-interfaces.xsl"

static int __reliably_save_to_disk(const char *outFilename, 
                                   enum relSaveToDiskModes mode, 
                                   xmlSession_t *sessionHandle)
{
    int status = SUCCESS;

    char tmpFilename[strlen(outFilename) + strlen(".XXXXXX") + 1];
    snprintf(tmpFilename, sizeof(tmpFilename), "%s.XXXXXX", outFilename);

    int tmpFd = mkstemp(tmpFilename);

    if(-1 == tmpFd)
    {
        perror("mkstemp");
        status = FILE_OPEN_ERROR;
    
        // Old configuration not changed yet -> discrepancy between XML and HW state
        // TODO: check for this error above ct_libnet and create an appropriate error message.
    }

    if(SUCCESS == status)
    {
        // the file created by mkstemp has 600 permissions, but 644 is needed for WBM
        if(-1 == fchmod(tmpFd, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH))
        {
            perror("fchmod");
            status = FILE_OPEN_ERROR;
        }
    }

    if(SUCCESS == status)
    {

        switch(mode)
        {
        case REL_SAVE_INTERFACES_XML:

            status = xmlSaveFormatFileEnc(tmpFilename, sessionHandle->xmlPtr, "UTF-8", 1);

            if(-1 == status)
            {
                status = FILE_WRITE_ERROR; 
                // Again, old config not yet changed (see above)
            }
            else
            {
                status = SUCCESS;
            }
            break;

        case REL_SAVE_ETC_NET_IFACES:
            status = ct_xslt_apply_transformation(sessionHandle->xmlPtr, NETWORK_INTERFACES_XSLT, tmpFd);
            
            // status != SUCCESS: XML config <-> HW state, but /etc/network/interfaces is old yet
            // Problem: ifdown/ifup would reset interface to old values
            // TODO: check for the codes above ct_libnet

            break;

        default:
            status = INVALID_PARAMETER;
            break;
        }
    }

    if(SUCCESS == status)
    {
        status = fsync(tmpFd);
        
        if(-1 == status)
        {
            // possible errnos: EIO: sync error (i.e. not enough free space)
            //                  EROFS, EINVAL: sync not supported (unlikely)
            //                  EBADF: Not a valid writable fd    (impossible, because it has already been written to above)

            perror("fsync");
            status = FILE_WRITE_ERROR; 
        }
        // sync fails: old config not touched
    }

    if(-1 != tmpFd)
    {
        if(-1 == close(tmpFd))
        {
        // At this point we cannot do anything, but our config might have been destroyed
        // Ultima ratio: reset to factory settings?
            perror("close");
            status = FILE_WRITE_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        struct stat linkStat;
        if (lstat(outFilename, &linkStat) < 0)
        {
            perror("stat");
            status = FILE_WRITE_ERROR;
        }
        else
        {
            if ( S_IFLNK != (linkStat.st_mode & S_IFMT) )
            {
                if(rename(tmpFilename, outFilename) == -1)
                {
                    perror("rename");
                    status = FILE_WRITE_ERROR;
                }
            }
            else
            {
                struct stat fileStat;
                if (stat(outFilename, &fileStat) < 0)
                {
                    perror("lstat");
                    status = FILE_WRITE_ERROR;
                }
                else
                {
                    char *linkname = g_malloc(fileStat.st_size + 1);
                    int len;
                    if (linkname)
                    {
                        len = readlink(outFilename, linkname, fileStat.st_size + 1);
                    }
                    else
                    {
                        len = -1;
                    }
                    if ((len < 0) || (len > fileStat.st_size))
                    {
                        perror("readlink");
                        status = FILE_WRITE_ERROR;
                    }
                    else
                    {
                        linkname[len] = '\0';
                        if(rename(tmpFilename, linkname) == -1)
                        {
                            perror("rename");
                            status = FILE_WRITE_ERROR;
                        }
                    }
                    if (linkname) g_free(linkname);
                }
            }
        }
    }

    return status;
}

// This commit also generates /etc/network/interfaces using XSLT. Use ct_xml_commit_xml_changes for
// commiting solely the XML file.
int ct_xml_commit_changes(xmlSession_t *sessionHandle)
{

    // ASSUMPTION: changes we want to commit are valid!

    int status = SUCCESS;

    assert(NULL != sessionHandle->xmlPtr);
    assert(NULL != sessionHandle->xmlPtr->URL);

    status = __reliably_save_to_disk((char*) sessionHandle->xmlPtr->URL, REL_SAVE_INTERFACES_XML, sessionHandle);

    // For ethernet layer settings:
    // At this point, if status == SUCCESS, all HW changes had been put to the main XML configuration.
    // This means, the settings will be transfered into HW after reboot => persistent and consistent state.
    //
    // Now, generate a new /etc/network/interfaces file to keep it consistent to the XML config.

    if(SUCCESS == status)
    {
        status = __reliably_save_to_disk("/etc/network/interfaces", REL_SAVE_ETC_NET_IFACES, sessionHandle);
    }

   return status;
}

int ct_xml_commit_xml_changes(xmlSession_t *sessionHandle)
{
    assert(NULL != sessionHandle->xmlPtr);
    assert(NULL != sessionHandle->xmlPtr->URL);

    return __reliably_save_to_disk((char*) sessionHandle->xmlPtr->URL, REL_SAVE_INTERFACES_XML, sessionHandle);
}

int ct_xml_generate_network_config(const char *outFilename, xmlSession_t *sessionHandle)
{
    int status = SUCCESS;

    assert(NULL != sessionHandle);

    int outFd = creat(outFilename, S_IRUSR | S_IWUSR);

    if(-1 == outFd)
    {
        status = FILE_OPEN_ERROR; 
    }
    else
    {
        xmlDocPtr doc = sessionHandle->xmlPtr;
        status = ct_xslt_apply_transformation(doc, NETWORK_INTERFACES_XSLT, outFd);   
        close(outFd); // TODO: check error
    }

    return status;
}

