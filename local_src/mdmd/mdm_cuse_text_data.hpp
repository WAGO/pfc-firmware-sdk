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
///  \file     mdm_cuse_text_data.hpp
///
///  \brief    Text data functions for MDMD CUSE.
///
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef MDM_CUSE_TEXT_DATA_H_
#define MDM_CUSE_TEXT_DATA_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <string>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define GSM_PDU_MSG_MAX_LENGTH                                               512

#define GSM_PDU_MSG_OK                                                        (0)
#define GSM_PDU_MSG_INCOMPLETE                                               (-1)
#define GSM_PDU_MSG_TOO_BIG                                                  (-2)
#define GSM_PDU_MSG_MISSING_LENGHT                                           (-3)

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Gets a single line from a buffer, terminated by <CR>/<LF>/<CR><LF>/<LF><CR>.
///
/// \param read_buffer
///   Buffer to read from.
/// \param line
///   String to write resulting line to.
///
/// \post
///   On success \param line contains read line without <CR>/<LF>/<CR><LF>/<LF><CR>,
///   and read line is removed from \param read_buffer.
///   On error \param line and \param read_buffer left untouched.
///
/// \return
///   Zero (0) in case of success, negative value otherwise.
//------------------------------------------------------------------------------
int get_line_from_buffer(std::string &read_buffer,
                         std::string &line);

//------------------------------------------------------------------------------
/// Gets a single PDU from a buffer, terminated by <CTRL-Z>/<ESC>.
///
/// \param read_buffer
///   Buffer to read from.
/// \param pdu
///   String to write resulting PDU to.
///
/// \post
///   On success \param pdu contains read PDU without <CTRL-Z>/<ESC>,
///   and read PDU is removed from \param read_buffer.
///   On error \param pdu and \param read_buffer left untouched.
///
/// \return
///   Zero (0) in case of success, negative value otherwise.
//------------------------------------------------------------------------------
int get_pdu_from_buffer(std::string &read_buffer,
                        std::string &pdu);

//------------------------------------------------------------------------------
/// Tries to get a single PDU from a buffer, terminated by <CTRL-Z>/<ESC>.
///
/// \param read_buffer
///   Buffer to read from.
/// \param pdu
///   String to write resulting PDU to.
///
/// \post
///   On success \param pdu contains read PDU without <CTRL-Z>/<ESC>,
///   and read PDU is removed from \param read_buffer.
///   On error \link GSM_PDU_MSG_INCOMPLETE \endlink \param pdu and \param read_buffer left untouched.
///   On any other error \param pdu is cleared and an completely read PDU is removed from \param read_buffer.
///
/// \return
///   \link GSM_PDU_MSG_OK \endlink in case of success,
///   \link GSM_PDU_MSG_INCOMPLETE \endlink in case of an empty buffer or an incomplete PDU,
///   \link GSM_PDU_MSG_TOO_BIG \endlink in case of a PDU exiting \link GSM_PDU_MSG_MAX_LENGTH \endlink,
///   \link GSM_PDU_MSG_MISSING_LENGHT \endlink in case of missing leading SMSC number length.
///
/// \see GSM_PDU_MSG_OK
/// \see GSM_PDU_MSG_INCOMPLETE
/// \see GSM_PDU_MSG_TOO_BIG
/// \see GSM_PDU_MSG_MISSING_LENGHT
//------------------------------------------------------------------------------
int try_to_get_pdu(std::string &read_buffer,
                   std::string &pdu);


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------


#endif /* MDM_CUSE_TEXT_DATA_H_ */
//---- End of source file ------------------------------------------------------

