//------------------------------------------------------------------------------
/// Copyright (c) 2020-2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Configuration tool to configure DNS and DHCP service.
///
///  \author   MSc, WAGO GmbH & Co. KG.
///  \author   MOe, WAGO GmbH & Co. KG.
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_CONFIG_DNSMASQ_HPP_
#define SRC_CONFIG_DNSMASQ_CONFIG_DNSMASQ_HPP_

namespace configdnsmasq {

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int execute(int argc, char **argv);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

}

#endif /* SRC_CONFIG_DNSMASQ_CONFIG_DNSMASQ_HPP_ */
