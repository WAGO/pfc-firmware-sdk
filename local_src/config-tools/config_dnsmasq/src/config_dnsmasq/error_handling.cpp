//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     error_handling.c
///
///  \brief    Error handling.
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
///  \author   MOe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include "error_handling.hpp"

#include <cassert>

::std::string erh_prgname;

/**
 * Table to convert error codes to text.
 */
static erh_code_to_message_t code_to_msg[] = {
    { MISSING_PARAMETER, "MISSING_PARAMETER" },
    { INVALID_PARAMETER, "INVALID_PARAMETER" },
    { FILE_OPEN_ERROR, "FILE_OPEN_ERROR" },
    { FILE_READ_ERROR, "FILE_READ_ERROR" },
    { FILE_CLOSE_ERROR, "FILE_CLOSE_ERROR" },
    { NOT_FOUND, "NOT_FOUND" },
    { SYSTEM_CALL_ERROR, "SYSTEM_CALL_ERROR" },
    { CONFIG_FILE_INCONSISTENT, "CONFIG_FILE_INCONSISTENT" },
    { TIMEOUT, "TIMEOUT" },
    { FILE_WRITE_ERROR, "FILE_WRITE_ERROR" },
    { NARROW_SPACE_WARNING, "NARROW_SPACE_WARNING" },
    { NOT_ENOUGH_SPACE_ERROR, "NOT_ENOUGH_SPACE_ERROR" },
    { SUCCESS, "UNDEFINED" }                   // End marker, don't remove.
};

void erh_init(const ::std::string &prgname) {
  erh_prgname = prgname;

  erh_code_to_message_t *p = code_to_msg;
  do {
    //printf("code: %d; %s\n", p->code, p->text);
    p++;
  } while (p->code != SUCCESS);
}

void erh_set_error(enum eStatusCode code, const std::string &message, bool exit_on_error) {
  erh_code_to_message_t *p = code_to_msg;

  do {
    if (p->code == code) {
      break;
    }
    p++;
  } while (p->code != SUCCESS);

  auto m = (boost::format("%s: " + message) % erh_prgname).str();

  ct_liblog_setLastError(m.c_str());

  fprintf(stderr, "%s: %s; code: %d\n", p->text, m.c_str(), code);

  if (exit_on_error) {
    exit(code);
  }
}

void erh_assert(bool condition, enum eStatusCode code, const std::string &message, bool exit_on_error) {
  if (!condition) {
    erh_set_error(code, message, exit_on_error);
  }
}

void erh_assert(bool condition, enum eStatusCode code, const boost::format &message, bool exit_on_error) {
  erh_assert(condition, code, message.str(), exit_on_error);
}
