// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include "timeout.h"

Timeout::Timeout(guint interval)
{
    _interval = interval;
    _source = g_timeout_source_new(_interval);
    g_source_set_callback(_source,Timeout::timeout_callback,this,0);
    g_source_attach( _source, 0);
}

Timeout::~Timeout()
{
    if (_source) {
        g_source_destroy(_source);
        g_source_unref(_source);
    }
}

gboolean
Timeout::timeout_callback(gpointer data)
{
    Timeout *to = static_cast<Timeout *>(data);
    to->callback();
    return true;
}

void
Timeout::kick_timeout( guint interval )
{

	if (_source != nullptr) {
			g_source_destroy(_source);
			g_source_unref(_source);
		}
    _source = g_timeout_source_new(interval);
    g_source_set_callback(_source,Timeout::timeout_callback,this,0);
    g_source_attach( _source, 0);
}
void
Timeout::kick_timeout()
{
    kick_timeout(_interval);
}

void
Timeout::deactivate()
{
	if (_source != nullptr) {
		g_source_destroy(_source);
		g_source_unref(_source);
	}
    _source = 0;
}

void
Timeout::callback()
{}
