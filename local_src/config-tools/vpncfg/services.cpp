// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "services.hpp"
#include "error.hpp"
#include "filesys.hpp"
#include "system.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace wago {
namespace services {


// static const char* get_name(const service s) noexcept
// {
//     return service::ipsec == s ? "ipsec" : "openvpn";
// }

static const char* get_init_name(const service s) noexcept
{
    return service::ipsec == s ? "/etc/init.d/ipsec" : "/etc/init.d/openvpn";
}

static const char* get_symlink_name(const service s) noexcept
{
    return service::ipsec == s ? "/etc/rc.d/S30_ipsec" : "/etc/rc.d/S30_openvpn";
}


bool is_enabled(const service s)
{
    return file_exists(get_init_name(s)) && symlink_exists(get_symlink_name(s));
}


void enable(const service s)
{
    if (is_enabled(s))
        return;

    const int ret = ::symlink(get_init_name(s), get_symlink_name(s));

    if (-1 == ret)
        throw system_call_error("Could not create services rc.d symlink.");
}


void disable(const service s)
{
    if (!symlink_exists(get_symlink_name(s)))
        return;

    const int ret = ::unlink(get_symlink_name(s));

    if (-1 == ret)
        throw system_call_error("Could not remove services rc.d symlink.");
}


// It can be specifically checked if there are active connections using
// ipsec status command - see first line of results, first number:
// "Security Associations (2 up, 0 connecting):"
static bool is_ipsec_running(void)
{
    const std::string res = exe_cmd("/usr/sbin/ipsec status 2> /dev/null");
    
    return 0 != res.length();
}


// With OpenVPN it is a bit harder as apparently there is no command to call
// and indirect methods must be used. For example one could use "ifconfig -a"
// and check for tun/tap interfaces. This method, howerver, is not bullet-proof
// and in the case of server ends it shows waiting interfaces, not necessarily
// connected.
static bool is_ovpn_running(void)
{
    const std::string res = exe_cmd("/bin/ps ax | /bin/grep \"openvpn.* --daemon\" | /bin/grep -v grep");

    return 0 != res.length();
}


bool is_running(const service s)
{
    return service::ipsec == s ? is_ipsec_running() : is_ovpn_running();
}


static void call_init(const service s, const std::string& cmd)
{
    const std::string init(get_init_name(s));

    if (!file_exists(init))
        throw system_call_error("It seems that the requrested vpn service is not installed.");

    (void)exe_cmd(init + " " + cmd); 
}


void start(const service s)
{
    call_init(s, "start");
}


void stop(const service s)
{
    call_init(s, "stop");
}


void restart(const service s)
{
    call_init(s, "reload");
}

void reload(const service s)
{
    call_init(s, "reload");
}


} // namespace services
} // namespace wago

