// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG



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

    stbuf.c_iflag &= static_cast<unsigned int>(~(IGNCR | ICRNL | IUCLC | INPCK | IXON | IXANY ));
    stbuf.c_oflag &= static_cast<unsigned int>(~(OPOST | OLCUC | OCRNL | ONLCR | ONLRET));
    stbuf.c_lflag &= static_cast<unsigned int>(~(ICANON | XCASE | ECHO | ECHOE | ECHONL));
    stbuf.c_cc[VMIN] = 1;
    stbuf.c_cc[VTIME] = 0;
    stbuf.c_cc[VEOF] = 1;

    /* Use software handshaking and ignore parity/framing errors */
    stbuf.c_iflag |= (IXON | IXOFF | IXANY | IGNPAR);

    /* Set up port speed and serial attributes; also ignore modem control
     * lines since most drivers don't implement RTS/CTS anyway.
     */

    const speed_t speed = B9600;
    if (cfgetispeed(&stbuf) != speed)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): set input speed 0x%08x (B9600)\n",
               __func__,_tty_fname.c_str(), speed);
      errno = 0;
      if (cfsetispeed (&stbuf, speed) != 0)
      {
          mdmd_Log(MD_LOG_ERR, "%s(%s): failed to set serial port input speed; errno %d\n",
              __func__,_tty_fname.c_str(), errno );
          throw SerialPortException();
      }
    }
    if (cfgetospeed(&stbuf) != speed)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): set output speed 0x%08x (B9600)\n",
               __func__,_tty_fname.c_str(), speed);
      errno = 0;
      if (cfsetospeed (&stbuf, speed) != 0)
      {
          mdmd_Log(MD_LOG_ERR, "%s(%s): failed to set serial port output speed; errno %d\n",
              __func__,_tty_fname.c_str(), errno );
          throw SerialPortException();
      }
    }
    tcflag_t flag_to_unset;
    tcflag_t flag_to_set;

    flag_to_set = CREAD;
    if ((stbuf.c_cflag & flag_to_set) == 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): set control flag CREAD\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag |= flag_to_set;
    }
    flag_to_set = CLOCAL;
    if ((stbuf.c_cflag & flag_to_set) == 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): set control flag CLOCAL\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag |= flag_to_set;
    }

    flag_to_unset = CSIZE;
    if ((stbuf.c_cflag & flag_to_unset) != 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): unset control flag CSIZE\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag &= ~flag_to_unset;
    }
    flag_to_set = CS8;
    if ((stbuf.c_cflag & flag_to_set) == 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): set control flag CS8\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag |= flag_to_set;
    }

    flag_to_unset = CSTOPB;
    if ((stbuf.c_cflag & flag_to_unset) != 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): unset control flag CSTOPB\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag &= ~flag_to_unset;
    }
    flag_to_unset = PARENB;
    if ((stbuf.c_cflag & flag_to_unset) != 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): unset control flag PARENB\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag &= ~flag_to_unset;
    }
    flag_to_unset = CRTSCTS;
    if ((stbuf.c_cflag & flag_to_unset) != 0)
    {
      mdmd_Log(MD_LOG_DBG, "%s(%s): unset control flag CRTSCTS\n", __func__,_tty_fname.c_str() );
      stbuf.c_cflag &= ~flag_to_unset;
    }

    if (tcsetattr (_fd, TCSANOW, &stbuf) < 0)
    {
        mdmd_Log(MD_LOG_ERR, "%s(%s): failed to set serial port attributes; errno %d\n",
            __func__,_tty_fname.c_str(), errno);
        throw SerialPortException();
    }

    /* tcsetattr() returns 0 if any of the requested attributes could be set,
     * so we should double-check that all expected values were set and log a warning if not.
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
        if (stbuf.c_iflag != other.c_iflag)
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): input mode flags 0x%08x not fully set: 0x%08x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_iflag, other.c_iflag);
        }
        if (stbuf.c_oflag != other.c_oflag)
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): output mode flags 0x%08x not fully set: 0x%08x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_oflag, other.c_oflag);
        }
        if (stbuf.c_cflag != other.c_cflag)
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): control mode flags 0x%08x not fully set: 0x%08x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_cflag, other.c_cflag);
        }
        if (stbuf.c_lflag != other.c_lflag)
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): local mode flags 0x%08x not fully set: 0x%08x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_lflag, other.c_lflag);
        }
        const speed_t ispeed = cfgetospeed(&other);
        if (ispeed != speed)
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): input speed 0x%08x not fully set: 0x%08x\n",
                     __func__,_tty_fname.c_str(), speed, ispeed);
        }
        const speed_t ospeed = cfgetospeed(&other);
        if (ospeed != speed)
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): output speed 0x%08x not fully set: 0x%08x\n",
                     __func__,_tty_fname.c_str(), speed, ospeed);
        }
        if (stbuf.c_cc[VMIN] != other.c_cc[VMIN])
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): control character VMIN 0x%02x not fully set: 0x%02x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_cc[VMIN], other.c_cc[VMIN]);
        }
        if (stbuf.c_cc[VTIME] != other.c_cc[VTIME])
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): control character VTIME 0x%02x not fully set: 0x%02x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_cc[VTIME], other.c_cc[VTIME]);
        }
        if (stbuf.c_cc[VEOF] != other.c_cc[VEOF])
        {
            mdmd_Log(MD_LOG_WRN, "%s(%s): control character VEOF 0x%02x not fully set: 0x%02x\n",
                     __func__,_tty_fname.c_str(), stbuf.c_cc[VEOF], other.c_cc[VEOF]);
        }
        //ignore other control characters that was not set
        //ignore line discipline (c_line) that was not set
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
    GError *err = nullptr;
    gsize written;
    GIOStatus status;

    std::string out = text + "\r\n";

    mdmd_Log(MD_LOG_DBG2, "write(%s): %s\n",_tty_fname.c_str(), text.c_str() );

    status = g_io_channel_write_chars( _channel, out.c_str(), -1, &written, &err );
    if (status != G_IO_STATUS_NORMAL)
    {
      if (err != nullptr)
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
    if (err != nullptr)
    {
      g_error_free(err);
    }
}

gboolean
SerialPort::io_watch_callback(GIOChannel *source, GIOCondition cond, gpointer data)
{
    SerialPort *o = (SerialPort *) data;

    try
    {
      o->io_watch(source, cond);
    }
    catch (...)
    {
      mdmd_Log(MD_LOG_WRN, "%s: modem port io error\n", __func__);
      o->io_error();
      return false;
    }

    return true;
}

void
SerialPort::io_watch(GIOChannel *source, GIOCondition cond)
{
    GError *err = nullptr;
    gsize read;
    GIOStatus status;

    (void)source; //unused parameter
    (void)cond; //unused parameter
    status = g_io_channel_read_chars( _channel, _io_buffer, sizeof(_io_buffer), &read, &err );

    if ((status != G_IO_STATUS_NORMAL) && (status != G_IO_STATUS_AGAIN) )
    {
      if (err != nullptr)
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
    if (err != nullptr)
    {
      g_error_free(err);
    }
    if (read > sizeof(_io_buffer))
    {
      mdmd_Log(MD_LOG_ERR, "read(%s): %u bytes exceeds buffer size %u\n", _tty_fname.c_str(), read, sizeof(_io_buffer));
      throw SerialPortException();
    }
    mdmd_Log(MD_LOG_DBG2, "read(%s): %u bytes\n", _tty_fname.c_str(), read );

    if (nullptr == _read_buffer) return;

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
    (void)text; //unused parameter
}
