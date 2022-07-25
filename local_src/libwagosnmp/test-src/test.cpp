//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test.cpp
///
///  \brief    Simple tests to show googletest working in PTXdist packet.
///
///  \author   Jens Breitenstein : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
extern "C"
{
#include "wagosnmp_API.h"
// It should not be necessary to include internal header, but in order to use libwagosnmp_Int32ToTlv,
// some definitions from asn are required.
#include "wagosnmp_internal.h"
}

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
TEST(Tlv, success)
{
  // setup

  // test
  ASSERT_TRUE(true);

  // teardown
}


TEST(Tlv, callSo)
{
  // setup

  // test
  ASSERT_EQ(WAGOSNMP_RETURN_BAD_DATATYPE, libwagosnmp_Int32ToTlv(0, ASN_BOOLEAN, NULL));

  // teardown
}


TEST(Tlv, CallSoWithParameter)
{
  // setup
  tWagoSnmpTlv tlvData;
  memset(&tlvData, 0, sizeof(tlvData));
  int32_t input = 12;

  // test
  ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_Int32ToTlv(input, ASN_INTEGER, &tlvData));

  // teardown
}


TEST(Tlv, Uninitialized_DeathTest)
{
  // setup
  tWagoSnmpTlv tlvData;
  memset(&tlvData, 0xCC, sizeof(tlvData));
  int32_t input = 12;

  // test
  ASSERT_DEATH(libwagosnmp_Int32ToTlv(input, ASN_INTEGER, &tlvData), "");

  // teardown
}


TEST(Tlv, Conversion)
{
  // setup
  tWagoSnmpTlv tlvData;
  memset(&tlvData, 0, sizeof(tlvData));
  int32_t input = 12;
  int32_t output = 0;

  // test
  ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_Int32ToTlv(input, ASN_INTEGER, &tlvData));
  ASSERT_EQ(WAGOSNMP_RETURN_OK, libwagosnmp_TlvToInt32(&tlvData, &output));
  ASSERT_EQ(input, output);

  // teardown
}


//---- End of source file ------------------------------------------------------

