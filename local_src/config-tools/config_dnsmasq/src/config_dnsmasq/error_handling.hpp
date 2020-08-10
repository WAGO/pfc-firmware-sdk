//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
///  \file     error_handling.c
///
///  \brief    Error handling.
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
///  \author   MOe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_ERROR_HANDLING_HPP_
#define SRC_CONFIG_DNSMASQ_ERROR_HANDLING_HPP_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <ct_liblog.h>

typedef struct {
    enum eStatusCode code;
    char const *text;
} erh_code_to_message_t;

// Set program name.
void erh_init(char *prgname);

// Error handling: Set error message for WBM and terminate program.
// The message may be a printf format string with exactly one %s which is substituted by the par string.
void erh_set_error(enum eStatusCode code, const char *msgfmt, const char *par);

// Assert a condition. If it is false, set error and terminate program.
void erh_assert(bool cond, enum eStatusCode code, const char *msgfmt, const char *par);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* SRC_CONFIG_DNSMASQ_ERROR_HANDLING_HPP_ */
