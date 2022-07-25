// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef MDM_FUNCTORS_H
#define MDM_FUNCTORS_H

#include "mdm_statemachine.h"

class dbus_swrite : public MdmStatemachine::transition_t::transistion_functor_t
{
    private:
      std::string _text;
    public:
      dbus_swrite( const std::string &text )
          : _text(text)
      {}
      bool operator () (MdmStatemachine &sm, State &src, State &dst, Event &ev) override {
          (void)src; (void)dst; //unused parameter
          DBusEvent *dev = dynamic_cast<DBusEvent *>(&ev);
          sm.set_current_invocation(dev->invocation());
          sm.write(_text);
          return true;
      }
};

class swrite : public MdmStatemachine::transition_t::transistion_functor_t
{
    private:
      std::string _text;
      unsigned int _timeout;
    public:
      swrite( const std::string &text, unsigned int timeout )
          : _text(text)
          , _timeout(timeout)
      {}
      bool operator () (MdmStatemachine &sm, State &src, State &dst, Event &ev) override {
          (void)src; (void)dst; (void)ev; //unused parameter
          sm.write(_text);
          sm.kick_cmd_timeout(_timeout);
          return true;
      }
};

#endif
