//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     diagnostic_min_API.h
///
///  \version <Revision>: $Rev: 65689 $
///
///  \brief    mini diagnostic API
///
///  \author   WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef DIAGNOSTIC_MIN_API_H_
#define DIAGNOSTIC_MIN_API_H_

//Changed from tEventId to tDiagnosticEventId because of type confilicts with Dal
typedef uint32_t log_tEventId ;

void log_EVENT_LogId(         log_tEventId           id,
                              bool               set);

#endif /* DIAGNOSTIC_MIN_API_H_ */
//---- End of source file ------------------------------------------------------

