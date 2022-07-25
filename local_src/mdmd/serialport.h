// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "glib.h"
#include <string>
#include <exception>
#include <queue>

class SerialPortException : public std::exception
{
    public:
	const char* what() const noexcept override {
	    return "SerialPortError";
	}

};

class SerialPort
{
    private:
  std::string _tty_fname;
	int _fd;
	GIOChannel *_channel;
	unsigned int _watch_id;

	gchar _io_buffer[1000];
	std::string * _read_buffer;

	static gboolean io_watch_callback(GIOChannel *source, GIOCondition cond, gpointer data);
	void io_watch(GIOChannel *source, GIOCondition cond);

    public:

	SerialPort( const std::string &fname, std::string *read_buffer );
	virtual ~SerialPort();

	void open(int fd);
	void open();
	void write( const std::string &text );
	virtual void read_line_cb( const std::string &line );
	virtual void io_error() = 0;
};

#endif
