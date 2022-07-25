//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file backup.hpp
/// 
/// \brief Backup and restore procedures, compatible with standard
///        backup & restore mechanism.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef WAGO_FIREWALL_BACKUP_HPP_
#define WAGO_FIREWALL_BACKUP_HPP_

namespace wago {
namespace firewall {


//------------------------------------------------------------------------------
/// Dump entire firewall's configuration on the stdio.
//------------------------------------------------------------------------------
void perform_backup(void);

//------------------------------------------------------------------------------
/// Restores entire firewall's configuration from the stdio.
//------------------------------------------------------------------------------
void perform_restore(void);

} // namespace firewall
} // namespace wago

#endif // WAGO_FIREWALL_BACKUP_HPP_
