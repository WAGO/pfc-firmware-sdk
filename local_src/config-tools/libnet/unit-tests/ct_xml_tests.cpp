//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 65689 $
///
/// \author ${user} $Author$ : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include "CppUTest/TestHarness.h"
#include <cstring>
#include <cstdio>
#include <cstdbool>

#include <glib.h>

#include "../ct_error_handling.h"

//#ifdef __UNIT_TESTS_HOST__  
//#endif

typedef struct xmlSession xmlSession_t;

extern "C" 
{
    int ct_xml_start_session(const char *xmlConfigFile, xmlSession_t **handle);
    int ct_xml_finish_session(xmlSession_t *handle);

    int ct_xml_set_value(const char * xpathExpr, const char * value, xmlSession_t *handle);
    int ct_xml_get_value(const char *xpathExpr, char *result, size_t resultLen, xmlSession_t *handle);

    int ct_xml_get_multiple_values(const char *xpathExpr, GString *result, const char *delim, xmlSession_t *handle);
    int ct_xml_set_multiple_values(const char *xpathExpr, const char *value, xmlSession_t *handle);
}

static xmlSession_t *g_xmlSession;

TEST_GROUP(ct_xml_init_cleanup)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST_GROUP(ct_xml_get_set)
{
    void setup()
    {
        (void) ct_xml_start_session("./file.xml", &g_xmlSession);
    }

    void teardown()
    {
        ct_xml_finish_session(g_xmlSession);
        g_xmlSession = NULL;
    }
};

IGNORE_TEST(ct_xml_get_set, ct_xml_test_asserts)
{
    char value[16];
    value[0] = '\0';

    // is prevented by an assert
    int status = ct_xml_get_value(NULL, value, sizeof(value), g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);

    status = ct_xml_get_value(NULL, NULL, 0, g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);

    status = ct_xml_set_value(NULL, NULL, g_xmlSession);
    LONGS_EQUAL(NOT_FOUND, status);

    status = ct_xml_set_value("/root/level1/level2_1/value", NULL, g_xmlSession);
    LONGS_EQUAL(NOT_FOUND, status);
}

TEST(ct_xml_init_cleanup, ct_xml_init_test)
{
    int status = SUCCESS;

    // test regular start/finish session
    g_xmlSession = NULL;
    status = ct_xml_start_session("./file.xml", &g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    CHECK(NULL != g_xmlSession);

    status = ct_xml_finish_session(g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
//    CHECK(NULL == g_xmlSession);

    // try to open a non-existant file
    g_xmlSession = NULL;
    status = ct_xml_start_session("bogus.xml", &g_xmlSession);
    LONGS_EQUAL(FILE_OPEN_ERROR, status);
//    CHECK(NULL == g_xmlSession);

    // try to pass an empty string
    status = ct_xml_start_session("", &g_xmlSession);
    LONGS_EQUAL(FILE_OPEN_ERROR, status);
}

TEST(ct_xml_init_cleanup, ct_xml_cleanup_test)
{
    // Try to finish a NULL session
    g_xmlSession = NULL;
    ct_xml_finish_session(g_xmlSession);
}

TEST(ct_xml_get_set, ct_xml_get_test)
{
    char value[16];
    value[0] = '\0';

    int status = ct_xml_get_value("/root/level1/level2_1/value", value, sizeof(value), g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("12345", value);

    value[0] = '\0';
    status = ct_xml_get_value("/root/level1/level2_1/value", value, 3, g_xmlSession);
    LONGS_EQUAL(NOT_ENOUGH_SPACE_ERROR, status);
    STRCMP_EQUAL("12", value);

    value[0] = '\0';
    status = ct_xml_get_value("BOGUS_XPATH", value, sizeof(value), g_xmlSession);
    LONGS_EQUAL(NOT_FOUND, status);

    status = ct_xml_set_value("/root/level1/level2_1/value", "54321", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);

    value[0] = '\0';
    status = ct_xml_get_value("/root/level1/level2_1/value", value, sizeof(value), g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("54321", value);

    status = ct_xml_set_value("BOGUS_XPATH", "54321", g_xmlSession);
    LONGS_EQUAL(NOT_FOUND, status);

    status = ct_xml_set_value("/root/level1/level2_1/empty_val", "abcd", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);

    value[0] = '\0';
    status = ct_xml_get_value("/root/level1/level2_1/empty_val", value, sizeof(value), g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("abcd", value);

    GString *gstrRes = g_string_new("");

    status = ct_xml_get_multiple_values("/root/level1/*/value", gstrRes, " ", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    // 12345 changed to 54321 above
    STRCMP_EQUAL("54321 23456", gstrRes->str);

    status = ct_xml_set_multiple_values("/root/level1/*/value", "XXXX", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);

    g_string_erase(gstrRes, 0, -1);
    status = ct_xml_get_multiple_values("/root/level1/*/value", gstrRes, " ", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("XXXX XXXX", gstrRes->str);

    g_string_erase(gstrRes, 0, -1);
    status = ct_xml_get_multiple_values("BOGUS_XPATH", gstrRes, " ", g_xmlSession);
    LONGS_EQUAL(NOT_FOUND, status);

    status = ct_xml_set_multiple_values("BOGUS_XPATH", "XXXX", g_xmlSession);
    LONGS_EQUAL(NOT_FOUND, status);

    status = ct_xml_set_multiple_values("/root/level1/*/empty_val", "ABCD", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);

    g_string_erase(gstrRes, 0, -1);
    status = ct_xml_get_multiple_values("//empty_val", gstrRes, " ", g_xmlSession);
    LONGS_EQUAL(SUCCESS, status);
    STRCMP_EQUAL("ABCD ABCD", gstrRes->str);

//////////////////////////////////////
    g_string_free(gstrRes, TRUE);
}

