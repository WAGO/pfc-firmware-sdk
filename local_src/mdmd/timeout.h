// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <glib.h>

class Timeout
{
    private:
	GSource *_source;
	guint _interval;

	static gboolean timeout_callback(gpointer data);

    public:
	Timeout(guint interval /*the timeout interval in milliseconds.*/);
	virtual ~Timeout();

	void kick_timeout();
	void kick_timeout(guint interval /*the timeout interval in milliseconds.*/);
	void deactivate();

	virtual void callback();
};


#endif
