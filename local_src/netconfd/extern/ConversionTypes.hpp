//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     ConversionTypes.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef EXTERN_CONVERSIONTYPES_HPP_
#define EXTERN_CONVERSIONTYPES_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

namespace netconfd {

using InterfaceList = ::std::vector<::std::string>;
using BridgeConfig = ::std::map<::std::string, InterfaceList>;

}

#endif /* EXTERN_CONVERSIONTYPES_HPP_ */
//---- End of source file ------------------------------------------------------

