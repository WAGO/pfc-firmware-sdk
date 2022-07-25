//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     cgroupcpuload.h
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef _CGROUPCPULOAD_H_
#define _CGROUPCPULOAD_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <inttypes.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

struct cgroupcpuload;


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


  struct cgroupcpuload * cgroupcpuload_init(char const * cgroup_path);

  void cgroupcpuload_destroy(struct cgroupcpuload * cgroupcpuload);

  uint32_t cgroupcpuload_get_load(struct cgroupcpuload * cgroupcpuload);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------



#endif /* _CGROUPCPULOAD_H_ */



//---- End of source file ------------------------------------------------------
