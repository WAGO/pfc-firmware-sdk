//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
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
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#ifndef WAGO_BACKUP_HPP
#define WAGO_BACKUP_HPP


namespace wago
{


//------------------------------------------------------------------------------
/// Dump entire firewall's configuration on the stdio.
//------------------------------------------------------------------------------
void perform_backup(void);

//------------------------------------------------------------------------------
/// Restores entire firewall's configuration from the stdio.
//------------------------------------------------------------------------------
void perform_restore(void);


} // namespace wago


#endif // WAGO_BACKUP_HPP

