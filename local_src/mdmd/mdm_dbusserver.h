// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef MDM_DBUSSERVER_H
#define MDM_DBUSSERVER_H

#include "dbus_server.h"

class MdmStatemachine;

class MdmDBusServer : public DBusServer
{
    private:
	MdmStatemachine *_statemachine;
    protected:
	void method_call (MethodInvocation invocation) override;
    void name_acquired(GDBusConnection *connection, const std::string &name) override;
    void name_lost(GDBusConnection *connection, const std::string &name) override;

    public:
	MdmDBusServer(MdmStatemachine *statemachine);
	~MdmDBusServer();

	void emit_diag_signal(unsigned int id, bool set, int intparam);
};


#endif
