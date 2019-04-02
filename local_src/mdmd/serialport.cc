// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG



#include "serialport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/serial.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <exception>
#include "mdmd_log.h"


SerialPort::SerialPort( const std::string &fname, std::string *read_buffer )
    : _tty_fname(fname)
    , _fd(-1)
    , _channel(0)
    , _watch_id(0)
    , _read_buffer(read_buffer)
{
}

SerialPort::~SerialPort()
{
    if (_watch_id)
    g_source_remove(_watch_id);

    if (_channel) {
    g_io_channel_shutdown(_channel,false,0);
    g_io_channel_unref(_channel);
    }

    if (_fd >= 0)
    ::close(_fd);
}

void
SerialPort::open()
{
    open( ::open( _tty_fname.c_str(), O_RDWR | O_NONBLOCK ) );
}

void
SerialPort::open(int fd)
{
    struct termios stbuf, other;

    _fd = fd;
    if (_fd < 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): invalid fd %d\n",
            __func__,_tty_fname.c_str(), _fd);
        throw SerialPortException();
    }
    memset (&stbuf, 0, sizeof (struct termios));
    if (tcgetattr (_fd, &stbuf) != 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to get current serial port attributes; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
        throw SerialPortException();
    }

    stbuf.c_iflag &= ~(IGNCR | ICRNL | IUCLC | INPCK | IXON | IXANY );
    stbuf.c_oflag &= ~(OPOST | OLCUC | OCRNL | ONLCR | ONLRET);
    stbuf.c_lflag &= ~(ICANON | XCASE | ECHO | ECHOE | ECHONL);
    stbuf.c_lflag &= ~(ECHO | ECHOE);
    stbuf.c_cc[VMIN] = 1;
    stbuf.c_cc[VTIME] = 0;
    stbuf.c_cc[VEOF] = 1;

    /* Use software handshaking and ignore parity/framing errors */
    stbuf.c_iflag |= (IXON | IXOFF | IXANY | IGNPAR);

    /* Set up port speed and serial attributes; also ignore modem control
     * lines since most drivers don't implement RTS/CTS anyway.
     */
    stbuf.c_cflag &= ~(CBAUD | CSIZE | CSTOPB | PARENB | CRTSCTS);
    stbuf.c_cflag |= (CS8 | CREAD | 0 | 0 | 0 | CLOCAL);

    errno = 0;
    if (cfsetispeed (&stbuf, B115200) != 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to set serial port input speed; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
        throw SerialPortException();
    }

    errno = 0;
    if (cfsetospeed (&stbuf, B115200) != 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to set serial port output speed; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
        throw SerialPortException();
    }

    if (tcsetattr (_fd, TCSANOW, &stbuf) < 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to set serial port attributes; errno %d\n",
            __func__,_tty_fname.c_str(), errno);
        throw SerialPortException();
    }

    /* tcsetattr() returns 0 if any of the requested attributes could be set,
     * so we should double-check that all were set and log a warning if not.
     */
    memset (&other, 0, sizeof (struct termios));
    errno = 0;
    if (tcgetattr (_fd, &other) != 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to get new serial port attributes; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
        throw SerialPortException();
    }

    if (memcmp (&stbuf, &other, sizeof (other)) != 0)
    {
        mdmd_Log(MD_LOG_WRN, "%s(%s): WARNING port attributes not fully set\n",
            __func__,_tty_fname.c_str());
        //throw SerialPortException();
    }

    tcflush(_fd, TCIOFLUSH);

    _channel = g_io_channel_unix_new(_fd);
    if (_channel == 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to create glib I/O channel; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
        throw SerialPortException();
    }

    g_io_channel_set_encoding (_channel, 0, 0);
    g_io_channel_set_buffered (_channel, false);
    _watch_id = g_io_add_watch (_channel, GIOCondition(G_IO_IN | G_IO_ERR | G_IO_HUP), io_watch_callback, this );
}

void
SerialPort::write (const std::string &text)
{
    GError *err = NULL;
    gsize written;
    GIOStatus status;

    std::string out = text + "\r\n";

    mdmd_Log(MD_LOG_DBG2, "write(%s): %s\n",_tty_fname.c_str(), text.c_str() );

    status = g_io_channel_write_chars( _channel, out.c_str(), -1, &written, &err );
    if (status != G_IO_STATUS_NORMAL)
    {
      if (err != NULL)
      {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to write glib I/O channel; %s\n",
            __func__,_tty_fname.c_str(), err->message);
        g_error_free(err);
      }
      else
      {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to write glib I/O channel; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
      }
      throw SerialPortException();
    }
    if (err != NULL)
    {
      g_error_free(err);
    }
}

gboolean
SerialPort::io_watch_callback(GIOChannel *source, GIOCondition cond, gpointer data)
{
    SerialPort *o = (SerialPort *) data;

    try {
    o->io_watch(source, cond);
    } catch (SerialPortException &e) {
    mdmd_Log(MD_LOG_WRN, "%s: modem port io error\n", __func__);
    o->io_error();
    return false;
    }

    return true;
}

void
SerialPort::io_watch(GIOChannel *source, GIOCondition cond)
{
    GError *err = NULL;
    gsize read;
    GIOStatus status;

    (void)source; //unused parameter
    (void)cond; //unused parameter
    status = g_io_channel_read_chars( _channel, _io_buffer, sizeof(_io_buffer), &read, &err );

    if ((status != G_IO_STATUS_NORMAL) && (status != G_IO_STATUS_AGAIN) )
    {
      if (err != NULL)
      {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to read glib I/O channel; %s\n",
            __func__,_tty_fname.c_str(), err->message);
        g_error_free(err);
      }
      else
      {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to read glib I/O channel; errno %d\n",
            __func__,_tty_fname.c_str(), errno );
      }
      throw SerialPortException();
    }
    if (err != NULL)
    {
      g_error_free(err);
    }
    if (read > sizeof(_io_buffer))
    {
      mdmd_Log(MD_LOG_ERR, "read(%s): %u bytes exceeds buffer size %d\n", _tty_fname.c_str(), read, sizeof(_io_buffer));
      throw SerialPortException();
    }
    mdmd_Log(MD_LOG_DBG2, "read(%s): %u bytes\n", _tty_fname.c_str(), read );

    if (NULL == _read_buffer) return;

    _read_buffer->append( _io_buffer,read );

    while (1)
    {
      if (_read_buffer->find("> ") == 0)
      {
        _read_buffer->erase(0,2);
        this->read_line_cb( "> " );
      }
      else
      {
        std::string line;
        size_t pos1;
        size_t pos2;
        size_t eol;

        pos1 = _read_buffer->find("\r");
        pos2 = _read_buffer->find("\n");

        if (pos1 == std::string::npos)
          eol = pos2;
        else if (pos2 == std::string::npos)
          eol = pos1;
        else
          eol = (pos1 < pos2) ? pos1 : pos2;

        if (eol == std::string::npos)
          break; //incomplete line
        line = _read_buffer->substr(0,eol);
        _read_buffer->erase(0,eol+1);

        if (line.length())
        {
          this->read_line_cb( line );
        }
      }
    }
}

void
SerialPort::read_line_cb( const std::string &text )
{
    mdmd_Log(MD_LOG_DBG2, "Read line: %s\n", text.c_str() );
}
