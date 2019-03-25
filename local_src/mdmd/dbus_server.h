// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef GDBUS_SERVER_H
#define GDBUS_SERVER_H

#include <gio/gio.h>
#include <string>


class MethodInvocation
{
    private:
	GDBusMethodInvocation *_invocation;

    public:
	MethodInvocation( GDBusMethodInvocation *invocation )
	    : _invocation(invocation)
	{}
	MethodInvocation()
	    : _invocation(0)
	{}

	~MethodInvocation() {}


	std::string sender() {
    const gchar *retval = (0==_invocation) ? NULL :
        g_dbus_method_invocation_get_sender(_invocation);
    return std::string((NULL==retval) ? "": retval);
	}

	std::string object_path() {
    const gchar *retval = (0==_invocation) ? NULL :
        g_dbus_method_invocation_get_object_path(_invocation);
    return std::string((NULL==retval) ? "": retval);
	}

	std::string interface_name() {
    const gchar *retval = (0==_invocation) ? NULL :
        g_dbus_method_invocation_get_interface_name(_invocation);
    return std::string((NULL==retval) ? "": retval);
	}

	std::string method_name() {
    const gchar *retval = (0==_invocation) ? NULL :
        g_dbus_method_invocation_get_method_name(_invocation);
    return std::string((NULL==retval) ? "": retval);
	}

	GVariant *parameters() {
	    return g_dbus_method_invocation_get_parameters(_invocation);
	}
	void return_value( GVariant *gvars) {
	    if (0!=_invocation)
	    {
	      g_dbus_method_invocation_return_value(_invocation, gvars);
	      //this call frees _invocation, you cannot use it afterwards
	      _invocation = 0;
	    }
	}
	void return_error( const std::string &err, const std::string &msg ) {
	    if (0!=_invocation)
	    {
	      g_dbus_method_invocation_return_dbus_error(_invocation, err.c_str(), msg.c_str());
        //this call frees _invocation, you cannot use it afterwards
        _invocation = 0;
	    }
	}
};

class DBusServer
{
    private:
	GDBusConnection *_connection;
	GDBusNodeInfo *_introspection_data;
	guint _owner_id;
	guint _registration_id;

	static const GDBusInterfaceVTable interface_vtable;

	static void handle_method_call (GDBusConnection       *connection,
		const gchar           *sender,
		const gchar           *object_path,
		const gchar           *interface_name,
		const gchar           *method_name,
		GVariant              *parameters,
		GDBusMethodInvocation *invocation,
		gpointer               user_data);

	static GVariant * handle_get_property (GDBusConnection  *connection,
		const gchar      *sender,
		const gchar      *object_path,
		const gchar      *interface_name,
		const gchar      *property_name,
		GError          **error,
		gpointer          user_data);

	static gboolean handle_set_property (GDBusConnection  *connection,
		const gchar      *sender,
		const gchar      *object_path,
		const gchar      *interface_name,
		const gchar      *property_name,
		GVariant         *value,
		GError          **error,
		gpointer          user_data);

	static void on_bus_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data);
	static void on_name_acquired (GDBusConnection *connection, const gchar *name, gpointer user_data);
	static void on_name_lost (GDBusConnection *connection, const gchar *name, gpointer user_data);

    protected:
  std::string _bus_name;
  std::string _object_name;
	bool _online;
	virtual void method_call (MethodInvocation invocation) = 0;

	virtual GVariant * get_property (GDBusConnection  *connection,
		const std::string      &sender,
		const std::string      &object_path,
		const std::string      &interface_name,
		const std::string      &property_name,
		GError		       **error)
	{
	  (void)connection; (void)sender; (void)object_path; (void)interface_name; (void)property_name; (void)error; //unused parameter
	  return 0;
	}

	virtual gboolean set_property (GDBusConnection  *connection,
		const std::string      &sender,
		const std::string      &object_path,
		const std::string      &interface_name,
		const std::string      &property_name,
		GVariant	       *value,
		GError		       **error)
	{
    (void)connection; (void)sender; (void)object_path; (void)interface_name; (void)property_name; (void)value; (void)error; //unused parameter
	  return false;
	}

	virtual void bus_acquired(GDBusConnection *connection, const std::string &name);
	virtual void name_acquired(GDBusConnection *connection, const std::string &name);
	virtual void name_lost(GDBusConnection *connection, const std::string &name);

    public:
	DBusServer( const std::string &bus_name, const std::string & object_name, const std::string & introspection_xml );
	virtual ~DBusServer();

	void emit_signal( const std::string &destination_bus_name,
			  const std::string &object_path,
			  const std::string &interface_name,
			  const std::string &signal_name,
			  GVariant *parameters);

	void broadcast_signal( const std::string &object_path,
			       const std::string &interface_name,
			       const std::string &signal_name,
			       GVariant *parameters);


	bool online() const { return _online; }
};

#endif
