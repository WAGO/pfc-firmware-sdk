// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H

#include <glib.h>
#include <string>

#ifndef MDMD_CONFIG_FILE
#define MDMD_CONFIG_FILE  "/etc/specific/mdmd_init.conf"
#endif

#define MDMD_CONFIG_FILE_SECTION  "DEFAULT"

class PersistentStorage
{
    private:
	GKeyFile *_gkeyfile;

    public:
	PersistentStorage();
	~PersistentStorage();

	void init_loglevel(void);
	int get_loglevel(void) const;
	bool set_loglevel(const int loglevel);

	bool get_port_enabled() const;
	void set_port_enabled(bool enabled);

	bool get_sim(std::string &iccid, std::string &pin) const;
	void set_sim(const std::string &iccid, const std::string &pin);

	bool get_selection(int &mode) const;
	void set_selection(const int mode);

	bool get_oper(int &id, int &act) const;
	void set_oper(int id, int act);
	void remove_oper();

	bool get_gprs_connectivity(int &state) const;
	void set_gprs_connectivity(const int state);

  void save();
};

#endif
