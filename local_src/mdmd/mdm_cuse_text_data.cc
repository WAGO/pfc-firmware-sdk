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
///  \file     mdm_cuse_text_data.cc
///
///  \brief    Text data functions for MDMD CUSE.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "mdm_cuse_text_data.hpp"

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

int get_line_from_buffer(std::string &read_buffer,
                         std::string &line)
{
  const char * szTerm = "\r\n"; //line ends with <CR> or <CR><LF>
  size_t pos_next;
  size_t pos_line_end = read_buffer.find_first_of(szTerm);
  if (pos_line_end == std::string::npos)
  {
    //line not complete
    return -1;
  }
  if (pos_line_end == 0)
  {
    //empty line
    line.clear();
  }
  else
  {
    line = read_buffer.substr(0, pos_line_end);
  }
  pos_next = read_buffer.find_first_not_of(szTerm, pos_line_end); //find first character of next line
  if (pos_next == std::string::npos)
  {
    read_buffer.clear();
  }
  else
  {
    read_buffer.erase(0, pos_next);
  }
  return 0;
}


int get_pdu_from_buffer(std::string &read_buffer,
                        std::string &pdu)
{
  const char * szTerm = "\x1a\x1b"; //SMS PDU ends with <CTRL+Z> or <ESC>
  size_t pos_pdu_end = read_buffer.find_first_of(szTerm);
  if (pos_pdu_end == std::string::npos)
  {
    //PDU not complete
    return -1;
  }
  if (pos_pdu_end == 0)
  {
    //empty PDU
    pdu.clear();
  }
  else
  {
    if (read_buffer.at(pos_pdu_end) == '\x1b')
    { //ESC aborts SMS send, return with an empty PDU in this case
      pdu.clear();
    }
    else
    {
      pdu = read_buffer.substr(0, pos_pdu_end);
    }
  }
  read_buffer.erase(0, (pos_pdu_end + 1));
  return 0;
}


int try_to_get_pdu(std::string &read_buffer,
                   std::string &pdu)
{
  int result = GSM_PDU_MSG_OK;

  if (0 != get_pdu_from_buffer(read_buffer, pdu))
  { //unable to get complete PDU
    if(    ((read_buffer.length() > 0) && (!isdigit(read_buffer[0])))
        || ((read_buffer.length() > 1) && (!isdigit(read_buffer[1]))))
    { //leading SMSC number length missing -> not a valid PDU, but maybe a tried command
      result = GSM_PDU_MSG_MISSING_LENGHT;
      pdu.clear();
    }
    else if(read_buffer.length() > GSM_PDU_MSG_MAX_LENGTH)
    { //body is too long
      result = GSM_PDU_MSG_TOO_BIG;
      pdu.clear();
    }
    else
    { //read_buffer is empty or PDU is not yet complete
      result = GSM_PDU_MSG_INCOMPLETE;
    }
  }
  else
  {
    if(    ((pdu.length() > 0) && (pdu.length() < 2))
        || ((pdu.length() > 0) && !isdigit(pdu[0]))
        || ((pdu.length() > 1) && !isdigit(pdu[1])))
    { //leading SMSC number length missing -> not a valid PDU, but maybe a tried command
      result = GSM_PDU_MSG_MISSING_LENGHT;
      pdu.clear();
    }
    if(pdu.length() > GSM_PDU_MSG_MAX_LENGTH)
    { //body is too long
      result = GSM_PDU_MSG_TOO_BIG;
      pdu.clear();
    }
  }

  return result;
}


//---- End of source file ------------------------------------------------------
