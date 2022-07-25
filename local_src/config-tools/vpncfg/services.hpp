// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef WAGO_CT_SERVICES_HPP
#define WAGO_CT_SERVICES_HPP


namespace wago {
namespace services {


enum class service
{
    ipsec,
    ovpn
};

bool is_enabled(const service s);
void enable(const service s);
void disable(const service s);
bool is_running(const service s);
void start(const service s);
void stop(const service s);
void restart(const service s);
void reload(const service s);


} // namespace services
} // namespace wago


#endif // WAGO_CT_SERVICES_HPP
