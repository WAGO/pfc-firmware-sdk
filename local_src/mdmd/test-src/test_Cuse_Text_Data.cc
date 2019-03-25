//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project mdmd (PTXdist package mdmd).
//
// Copyright (c) 2017 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_Cuse_Text_Data.cc
///
///  \brief    Test for MDMD CUSE text data.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include "../mdm_cuse_text_data.hpp"

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define TEST_STD_TEXT                                                "Test Text"
#define TEST_STD_TRAILING_TEXT                                   "Trailing Data"

#define TEST_BYTES_512 \
  "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" \
  "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" \
  "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" \
  "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" \
  "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" \
  "0123456789abc"


struct DataInputTestParams
{
  char const * const szInput;
  char const * const szResult;
  char const * const szLeftInput;
};

class DataInputTest : public ::testing::TestWithParam<struct DataInputTestParams>
{
  public:
    std::string inputBuffer;

  protected:
    DataInputTest() {};
    ~DataInputTest() override {};

    void SetUp() override
    {
      // Prepare input data
      this->inputBuffer.clear();
      this->inputBuffer += GetParam().szInput;
    }

    void TearDown() override
    {
      // Nothing to do yet
    }
};

class LineInputTest : public DataInputTest
{
  protected:
    LineInputTest() {};
    ~LineInputTest() override {};
};

class LineInputErrorTest : public LineInputTest
{
  protected:
    LineInputErrorTest() {};
    ~LineInputErrorTest() override {};
};

class PduInputTest : public DataInputTest
{
  protected:
    PduInputTest() {};
    ~PduInputTest() override {};
};

class PduInputErrorTest : public PduInputTest
{
  protected:
    PduInputErrorTest() {};
    ~PduInputErrorTest() override {};
};

class PduGetTest : public DataInputTest
{
  protected:
    PduGetTest() {};
    ~PduGetTest() override {};
};

class PduGetIncompleteErrorTest : public PduGetTest
{
  protected:
    PduGetIncompleteErrorTest() {};
    ~PduGetIncompleteErrorTest() override {};
};

class PduGetToBigErrorTest : public PduGetTest
{
  protected:
    PduGetToBigErrorTest() {};
    ~PduGetToBigErrorTest() override {};
};

class PduGetWithoutLengthErrorTest : public PduGetTest
{
  protected:
    PduGetWithoutLengthErrorTest() {};
    ~PduGetWithoutLengthErrorTest() override {};
};


//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------

TEST_P(LineInputTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(LineInputTest, GetLine)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_EQ(0, get_line_from_buffer(this->inputBuffer, resultString));
  EXPECT_STREQ(params.szResult, resultString.c_str());
  EXPECT_STREQ(params.szLeftInput, this->inputBuffer.c_str());
}


static struct DataInputTestParams const arValidEmptyLinesCR[] =
{
  { "\r",                                         "",                        "" },
  { "\r" TEST_STD_TRAILING_TEXT,                  "", TEST_STD_TRAILING_TEXT "" },
  { "\r" TEST_STD_TRAILING_TEXT "\r",             "", TEST_STD_TRAILING_TEXT "\r" },
  { "\r" TEST_STD_TRAILING_TEXT "\n",             "", TEST_STD_TRAILING_TEXT "\n" },
  { "\r" TEST_STD_TRAILING_TEXT "\r\n",           "", TEST_STD_TRAILING_TEXT "\r\n" },
  { "\r" TEST_STD_TRAILING_TEXT "\n\r",           "", TEST_STD_TRAILING_TEXT "\n\r" },
  { "\r" TEST_STD_TRAILING_TEXT "\rTrailer2",     "", TEST_STD_TRAILING_TEXT "\rTrailer2" }
};

static struct DataInputTestParams const arValidEmptyLinesLF[] =
{
  { "\n",                                         "",                        "" },
  { "\n" TEST_STD_TRAILING_TEXT "",               "", TEST_STD_TRAILING_TEXT "" },
  { "\n" TEST_STD_TRAILING_TEXT "\r",             "", TEST_STD_TRAILING_TEXT "\r" },
  { "\n" TEST_STD_TRAILING_TEXT "\n",             "", TEST_STD_TRAILING_TEXT "\n" },
  { "\n" TEST_STD_TRAILING_TEXT "\r\n",           "", TEST_STD_TRAILING_TEXT "\r\n" },
  { "\n" TEST_STD_TRAILING_TEXT "\n\r",           "", TEST_STD_TRAILING_TEXT "\n\r" },
  { "\n" TEST_STD_TRAILING_TEXT "\nTrailer2",     "", TEST_STD_TRAILING_TEXT "\nTrailer2" }
};

static struct DataInputTestParams const arValidEmptyLinesCRLF[] =
{
  { "\r\n",                                       "",                        "" },
  { "\r\n" TEST_STD_TRAILING_TEXT "",             "", TEST_STD_TRAILING_TEXT "" },
  { "\r\n" TEST_STD_TRAILING_TEXT "\r",           "", TEST_STD_TRAILING_TEXT "\r" },
  { "\r\n" TEST_STD_TRAILING_TEXT "\n",           "", TEST_STD_TRAILING_TEXT "\n" },
  { "\r\n" TEST_STD_TRAILING_TEXT "\r\n",         "", TEST_STD_TRAILING_TEXT "\r\n" },
  { "\r\n" TEST_STD_TRAILING_TEXT "\n\r",         "", TEST_STD_TRAILING_TEXT "\n\r" },
  { "\r\n" TEST_STD_TRAILING_TEXT "\r\nTrailer2", "", TEST_STD_TRAILING_TEXT "\r\nTrailer2" }
};

static struct DataInputTestParams const arValidEmptyLinesLFCR[] =
{
  { "\n\r",                                       "",                        "" },
  { "\n\r" TEST_STD_TRAILING_TEXT "",             "", TEST_STD_TRAILING_TEXT "" },
  { "\n\r" TEST_STD_TRAILING_TEXT "\r",           "", TEST_STD_TRAILING_TEXT "\r" },
  { "\n\r" TEST_STD_TRAILING_TEXT "\n",           "", TEST_STD_TRAILING_TEXT "\n" },
  { "\n\r" TEST_STD_TRAILING_TEXT "\r\n",         "", TEST_STD_TRAILING_TEXT "\r\n" },
  { "\n\r" TEST_STD_TRAILING_TEXT "\n\r",         "", TEST_STD_TRAILING_TEXT "\n\r" },
  { "\n\r" TEST_STD_TRAILING_TEXT "\n\rTrailer2", "", TEST_STD_TRAILING_TEXT "\n\rTrailer2" }
};

INSTANTIATE_TEST_CASE_P(EmptyLineCR, LineInputTest, ::testing::ValuesIn(arValidEmptyLinesCR));
INSTANTIATE_TEST_CASE_P(EmptyLineLF, LineInputTest, ::testing::ValuesIn(arValidEmptyLinesLF));
INSTANTIATE_TEST_CASE_P(EmptyLineCRLF, LineInputTest, ::testing::ValuesIn(arValidEmptyLinesCRLF));
INSTANTIATE_TEST_CASE_P(EmptyLineLFCR, LineInputTest, ::testing::ValuesIn(arValidEmptyLinesLFCR));


static struct DataInputTestParams const arValidTestLinesCR[] =
{
  { TEST_STD_TEXT "\r",                                         TEST_STD_TEXT "",                        "" },
  { TEST_STD_TEXT "\r" TEST_STD_TRAILING_TEXT,                  TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "" },
  { TEST_STD_TEXT "\r" TEST_STD_TRAILING_TEXT "\r",             TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r" },
  { TEST_STD_TEXT "\r" TEST_STD_TRAILING_TEXT "\n",             TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n" },
  { TEST_STD_TEXT "\r" TEST_STD_TRAILING_TEXT "\r\n",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r\n" },
  { TEST_STD_TEXT "\r" TEST_STD_TRAILING_TEXT "\n\r",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n\r" },
  { TEST_STD_TEXT "\r" TEST_STD_TRAILING_TEXT "\rTrailer2",     TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\rTrailer2" }
};

static struct DataInputTestParams const arValidTestLinesLF[] =
{
  { TEST_STD_TEXT "\n",                                         TEST_STD_TEXT "",                        "" },
  { TEST_STD_TEXT "\n" TEST_STD_TRAILING_TEXT "",               TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "" },
  { TEST_STD_TEXT "\n" TEST_STD_TRAILING_TEXT "\r",             TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r" },
  { TEST_STD_TEXT "\n" TEST_STD_TRAILING_TEXT "\n",             TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n" },
  { TEST_STD_TEXT "\n" TEST_STD_TRAILING_TEXT "\r\n",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r\n" },
  { TEST_STD_TEXT "\n" TEST_STD_TRAILING_TEXT "\n\r",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n\r" },
  { TEST_STD_TEXT "\n" TEST_STD_TRAILING_TEXT "\nTrailer2",     TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\nTrailer2" }
};

static struct DataInputTestParams const arValidTestLinesCRLF[] =
{
  { TEST_STD_TEXT "\r\n",                                       TEST_STD_TEXT "",                        "" },
  { TEST_STD_TEXT "\r\n" TEST_STD_TRAILING_TEXT "",             TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "" },
  { TEST_STD_TEXT "\r\n" TEST_STD_TRAILING_TEXT "\r",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r" },
  { TEST_STD_TEXT "\r\n" TEST_STD_TRAILING_TEXT "\n",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n" },
  { TEST_STD_TEXT "\r\n" TEST_STD_TRAILING_TEXT "\r\n",         TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r\n" },
  { TEST_STD_TEXT "\r\n" TEST_STD_TRAILING_TEXT "\n\r",         TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n\r" },
  { TEST_STD_TEXT "\r\n" TEST_STD_TRAILING_TEXT "\r\nTrailer2", TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r\nTrailer2" }
};

static struct DataInputTestParams const arValidTestLinesLFCR[] =
{
  { TEST_STD_TEXT "\n\r",                                       TEST_STD_TEXT "",                        "" },
  { TEST_STD_TEXT "\n\r" TEST_STD_TRAILING_TEXT "",             TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "" },
  { TEST_STD_TEXT "\n\r" TEST_STD_TRAILING_TEXT "\r",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r" },
  { TEST_STD_TEXT "\n\r" TEST_STD_TRAILING_TEXT "\n",           TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n" },
  { TEST_STD_TEXT "\n\r" TEST_STD_TRAILING_TEXT "\r\n",         TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\r\n" },
  { TEST_STD_TEXT "\n\r" TEST_STD_TRAILING_TEXT "\n\r",         TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n\r" },
  { TEST_STD_TEXT "\n\r" TEST_STD_TRAILING_TEXT "\n\rTrailer2", TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\n\rTrailer2" }
};

INSTANTIATE_TEST_CASE_P(TestLineCR, LineInputTest, ::testing::ValuesIn(arValidTestLinesCR));
INSTANTIATE_TEST_CASE_P(TestLineLF, LineInputTest, ::testing::ValuesIn(arValidTestLinesLF));
INSTANTIATE_TEST_CASE_P(TestLineCRLF, LineInputTest, ::testing::ValuesIn(arValidTestLinesCRLF));
INSTANTIATE_TEST_CASE_P(TestLineLFCR, LineInputTest, ::testing::ValuesIn(arValidTestLinesLFCR));


TEST_P(LineInputErrorTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(LineInputErrorTest, GetLine)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_NE(0, get_line_from_buffer(this->inputBuffer, resultString));
  EXPECT_STREQ("", resultString.c_str());
  EXPECT_STREQ(params.szInput, this->inputBuffer.c_str());
}


static struct DataInputTestParams const arInvalidTestLines[] =
{
  { "",                                           "", "" },
  { TEST_STD_TEXT "",                             "", "" },
  { TEST_STD_TEXT "\t" TEST_STD_TRAILING_TEXT "", "", "" },
  { TEST_STD_TEXT " " TEST_STD_TRAILING_TEXT "",  "", "" }
};

INSTANTIATE_TEST_CASE_P(TestInvalidLine, LineInputErrorTest, ::testing::ValuesIn(arInvalidTestLines));


TEST_P(PduInputTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(PduInputTest, GetPdu)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_EQ(0, get_pdu_from_buffer(this->inputBuffer, resultString));
  EXPECT_STREQ(params.szResult, resultString.c_str());
  EXPECT_STREQ(params.szLeftInput, this->inputBuffer.c_str());
}


static struct DataInputTestParams const arValidEmptyPDUsCTRL_Z[] =
{
  {                    "\x1a",                                       "",                        "" },
  {                    "\x1a" TEST_STD_TRAILING_TEXT,                "", TEST_STD_TRAILING_TEXT "" },
  {                    "\x1a" TEST_STD_TRAILING_TEXT "\x1a",         "", TEST_STD_TRAILING_TEXT "\x1a" },
  {                    "\x1a" TEST_STD_TRAILING_TEXT "\x1b",         "", TEST_STD_TRAILING_TEXT "\x1b" },
  {                    "\x1a" TEST_STD_TRAILING_TEXT "\x1aTrailer2", "", TEST_STD_TRAILING_TEXT "\x1aTrailer2" }
};

static struct DataInputTestParams const arValidEmptyPDUsESC[] =
{
  {                    "\x1b",                                       "",                        "" },
  {                    "\x1b" TEST_STD_TRAILING_TEXT,                "", TEST_STD_TRAILING_TEXT "" },
  {                    "\x1b" TEST_STD_TRAILING_TEXT "\x1a",         "", TEST_STD_TRAILING_TEXT "\x1a" },
  {                    "\x1b" TEST_STD_TRAILING_TEXT "\x1b",         "", TEST_STD_TRAILING_TEXT "\x1b" },
  {                    "\x1b" TEST_STD_TRAILING_TEXT "\x1bTrailer2", "", TEST_STD_TRAILING_TEXT "\x1bTrailer2" },
  { "00" TEST_STD_TEXT "\x1b",                                       "",                        "" },
  { "00" TEST_STD_TEXT "\x1b" TEST_STD_TRAILING_TEXT,                "", TEST_STD_TRAILING_TEXT "" },
  { "00" TEST_STD_TEXT "\x1b" TEST_STD_TRAILING_TEXT "\x1a",         "", TEST_STD_TRAILING_TEXT "\x1a" },
  { "00" TEST_STD_TEXT "\x1b" TEST_STD_TRAILING_TEXT "\x1b",         "", TEST_STD_TRAILING_TEXT "\x1b" },
  { "00" TEST_STD_TEXT "\x1b" TEST_STD_TRAILING_TEXT "\x1bTrailer2", "", TEST_STD_TRAILING_TEXT "\x1bTrailer2" }
};

INSTANTIATE_TEST_CASE_P(EmptyPDU_CTRL_Z, PduInputTest, ::testing::ValuesIn(arValidEmptyPDUsCTRL_Z));
INSTANTIATE_TEST_CASE_P(EmptyPDU_ESC, PduInputTest, ::testing::ValuesIn(arValidEmptyPDUsESC));


static struct DataInputTestParams const arValidPDUs[] =
{
  { "00" TEST_STD_TEXT "\x1a",                                                            "00" TEST_STD_TEXT "",                                             "" },
  { "00" TEST_STD_TEXT "\r\x1a"                    TEST_STD_TRAILING_TEXT,                "00" TEST_STD_TEXT "\r",                    TEST_STD_TRAILING_TEXT "" },
  { "00" TEST_STD_TEXT "\n\x1a"                    TEST_STD_TRAILING_TEXT "\x1a",         "00" TEST_STD_TEXT "\n",                    TEST_STD_TRAILING_TEXT "\x1a" },
  { "00" TEST_STD_TEXT "\r\n\x1a"                  TEST_STD_TRAILING_TEXT "\x1b",         "00" TEST_STD_TEXT "\r\n",                  TEST_STD_TRAILING_TEXT "\x1b" },
  { "00" TEST_STD_TEXT "\n\r\x1a"                  TEST_STD_TRAILING_TEXT "\x1aTrailer2", "00" TEST_STD_TEXT "\n\r",                  TEST_STD_TRAILING_TEXT "\x1aTrailer2" },
  { "00" TEST_STD_TEXT "\r\n" TEST_STD_TEXT "\x1a" TEST_STD_TRAILING_TEXT "\x1bTrailer2", "00" TEST_STD_TEXT "\r\n" TEST_STD_TEXT "", TEST_STD_TRAILING_TEXT "\x1bTrailer2" }
};

INSTANTIATE_TEST_CASE_P(TestPDU, PduInputTest, ::testing::ValuesIn(arValidPDUs));


TEST_P(PduInputErrorTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(PduInputErrorTest, GetPdu)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_NE(0, get_pdu_from_buffer(this->inputBuffer, resultString));
  EXPECT_STREQ("", resultString.c_str());
  EXPECT_STREQ(params.szInput, this->inputBuffer.c_str());
}


static struct DataInputTestParams const arIncompletePDUs[] =
{
  { "00"              "",     "", "" },
  { "00"               " ",    "", "" },
  { "00"               "\t",   "", "" },
  { "00"               "\r",   "", "" },
  { "00"               "\n",   "", "" },
  { "00"               "\r\n", "", "" },
  { "00"               "\n\r", "", "" },
  { "00" TEST_STD_TEXT "",     "", "" },
  { "00" TEST_STD_TEXT " ",    "", "" },
  { "00" TEST_STD_TEXT "\t",   "", "" },
  { "00" TEST_STD_TEXT "\r",   "", "" },
  { "00" TEST_STD_TEXT "\n",   "", "" },
  { "00" TEST_STD_TEXT "\r\n", "", "" },
  { "00" TEST_STD_TEXT "\n\r", "", "" }
};

INSTANTIATE_TEST_CASE_P(TestInvalidPDU, PduInputErrorTest, ::testing::ValuesIn(arIncompletePDUs));


TEST_P(PduGetTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(PduGetTest, GetPdu)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_EQ(GSM_PDU_MSG_OK, try_to_get_pdu(this->inputBuffer, resultString));
  EXPECT_STREQ(params.szResult, resultString.c_str());
  EXPECT_STREQ(params.szLeftInput, this->inputBuffer.c_str());
}

INSTANTIATE_TEST_CASE_P(EmptyPDU_CTRL_Z, PduGetTest, ::testing::ValuesIn(arValidEmptyPDUsCTRL_Z));
INSTANTIATE_TEST_CASE_P(EmptyPDU_ESC, PduGetTest, ::testing::ValuesIn(arValidEmptyPDUsESC));
INSTANTIATE_TEST_CASE_P(TestPDU, PduGetTest, ::testing::ValuesIn(arValidPDUs));


TEST_P(PduGetIncompleteErrorTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(PduGetIncompleteErrorTest, GetPdu)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_EQ(GSM_PDU_MSG_INCOMPLETE, try_to_get_pdu(this->inputBuffer, resultString));
  EXPECT_STREQ("", resultString.c_str());
  EXPECT_STREQ(params.szInput, this->inputBuffer.c_str());
}

INSTANTIATE_TEST_CASE_P(TestInvalidPDU, PduGetIncompleteErrorTest, ::testing::ValuesIn(arIncompletePDUs));


TEST_P(PduGetToBigErrorTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(PduGetToBigErrorTest, GetPdu)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_EQ(GSM_PDU_MSG_TOO_BIG, try_to_get_pdu(this->inputBuffer, resultString));
  EXPECT_STREQ("", resultString.c_str());
  EXPECT_STREQ(params.szLeftInput, this->inputBuffer.c_str());
}


static struct DataInputTestParams const arPDUsToBig[] =
{
  { TEST_BYTES_512 "\x1a",                                           "",                        "" },
  { "00" TEST_STD_TEXT TEST_BYTES_512 "\x1a",                        "",                        "" },
  { TEST_BYTES_512 "\x1a" TEST_STD_TRAILING_TEXT,                    "", TEST_STD_TRAILING_TEXT "" },
  { "00" TEST_STD_TEXT TEST_BYTES_512 "\x1a" TEST_STD_TRAILING_TEXT, "", TEST_STD_TRAILING_TEXT "" },
  // We need also test incomplete PDUs with a too large body (=>WAT25807)
  { TEST_BYTES_512 "",                                               "",                        TEST_BYTES_512 "" },
  { "00" TEST_STD_TEXT TEST_BYTES_512 "" ,                           "",                        "00" TEST_STD_TEXT TEST_BYTES_512 "" }
};

INSTANTIATE_TEST_CASE_P(TestInvalidPDU, PduGetToBigErrorTest, ::testing::ValuesIn(arPDUsToBig));


TEST_P(PduGetWithoutLengthErrorTest, CheckSetup)
{
  // Nothing to do here, everything is done in SetUp/TearDown
}


TEST_P(PduGetWithoutLengthErrorTest, GetPdu)
{
  struct DataInputTestParams const & params = GetParam();
  std::string resultString;

  ASSERT_EQ(GSM_PDU_MSG_MISSING_LENGHT, try_to_get_pdu(this->inputBuffer, resultString));
  EXPECT_STREQ("", resultString.c_str());
  EXPECT_STREQ(params.szLeftInput, this->inputBuffer.c_str());
}


static struct DataInputTestParams const arPDUsWithoutLength[] =
{
 { TEST_STD_TEXT "\x1a",                      "", "" },
 { TEST_STD_TEXT "\r\x1a",                    "", "" },
 { TEST_STD_TEXT "\n\x1a",                    "", "" },
 { TEST_STD_TEXT "\r\n\x1a",                  "", "" },
 { TEST_STD_TEXT "\n\r\x1a",                  "", "" },
 { TEST_STD_TEXT "\r\n" TEST_STD_TEXT "\x1a", "", "" },
 // We need also test incomplete PDUs without length information (=>WAT25807)
 { TEST_STD_TEXT "",                      "", TEST_STD_TEXT "" },
 { TEST_STD_TEXT "\r",                    "", TEST_STD_TEXT "\r" },
 { TEST_STD_TEXT "\n",                    "", TEST_STD_TEXT "\n" },
 { TEST_STD_TEXT "\r\n",                  "", TEST_STD_TEXT "\r\n" },
 { TEST_STD_TEXT "\n\r",                  "", TEST_STD_TEXT "\n\r" },
 { TEST_STD_TEXT "\r\n" TEST_STD_TEXT "", "", TEST_STD_TEXT "\r\n" TEST_STD_TEXT "" }
};

INSTANTIATE_TEST_CASE_P(TestInvalidPDU, PduGetWithoutLengthErrorTest, ::testing::ValuesIn(arPDUsWithoutLength));


//---- End of source file ------------------------------------------------------

