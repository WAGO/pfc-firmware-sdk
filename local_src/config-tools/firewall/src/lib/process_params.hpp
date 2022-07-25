//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     process_params.hpp
///
///  \brief    Function processing params configuration file, defining network device name mappings, e.g. ethX1 <-> br0.
///
///  \author   WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef WAGO_FIREWALL_PROCESS_PARAMS_HPP_
#define WAGO_FIREWALL_PROCESS_PARAMS_HPP_

#include "file_accessor.hpp"

namespace wago {
namespace firewall {

//------------------------------------------------------------------------------
/// Update params_gen.xml file, defining network device name mappings, e.g. ethX1 <-> br0.
///
/// The params_gen.xml file bases on the params.xml file. In addition, the ipcmn.xml file
/// is checked for ip masquerading and port forwarding rules concerning network interfaces
/// which are not part of the params_gen.xml file yet. If such network interfaces are found,
/// they are added to params_gen.xml file.
///
//------------------------------------------------------------------------------
void update_network_interface_name_mapping(const FileAccessor& file_accessor);

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_PROCESS_PARAMS_HPP_
