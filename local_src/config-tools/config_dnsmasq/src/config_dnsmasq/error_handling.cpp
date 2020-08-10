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

#include <glib.h>
#include <cassert>

static char const *erh_prgname = "";
static size_t erh_prgnamesize;

// Table to convert error codes to text.
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

// Set program name.
void erh_init(char *prgname) {
  erh_prgname = g_strdup(prgname);
  erh_prgnamesize = strlen(erh_prgname);
}

// Error handling: Set error message for WBM and terminate program.
// The message may be a printf format string with exactly one %s which is substituted by the par string.
void erh_set_error(enum eStatusCode code, const char *msgfmt, const char *par) {
  erh_code_to_message_t *p = code_to_msg;
  static char message[256];
  message[0] = '\0';

  do {
    if (p->code == code) {
      break;
    }
    p++;
  } while (p->code != SUCCESS);

  int slen = snprintf(message, sizeof(message), "%s: ", erh_prgname);
  slen += snprintf(message + slen, /*(size_t)*/((int)sizeof(message) - slen), msgfmt, par);
  assert(slen <= (int)sizeof(message));

  ct_liblog_setLastError(message);

  fprintf(stderr, "%s: %s\n", p->text, message);
  exit(code);
}

// Assert a condition. If it is false, set error and terminate program.
void erh_assert(bool cond, enum eStatusCode code, const char *msgfmt, const char *par) {
  if (!cond) {
    erh_set_error(code, msgfmt, par);
  }
}
