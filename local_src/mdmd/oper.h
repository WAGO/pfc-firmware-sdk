// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef OPER_H
#define OPER_H

#include <stdio.h>

#define RSSI_MAX       31
#define RSSI_UNDEF     99
#define BER_UNDEF      99

#define OPER_STATE_UNKNOWN  0
#define OPER_STATE_ALLOWED  1
#define OPER_STATE_CURRENT  2
#define OPER_STATE_DENIED   3



//mapping of asu to quality steps:
// 0 = no net
// 1 = GSM only    / UMTS only
// 2 = GPRS slow   / HSDPA instable but possible
// 3 = GPRS fast   / HSDPA slow
// 4 = EDGE stable / HSDPA stable
// 5 = EDGE fast   / HSDPA fast /
// 6 = E-EDGE      / HSPA+
static int asu_mapping_2g[32]={0,1,1,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6};
static int asu_mapping_3g[32]={0,1,1,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6};

class Oper
{
    private:
    int         _selection_mode;
    int         _id;
    int         _act;
    int         _state;
    int         _ber;
    int         _rssi;

    public:
    Oper()
        : _selection_mode(-1)
        , _id(0)
        , _act(-1)
        , _state(OPER_STATE_UNKNOWN)
        , _ber(BER_UNDEF)
        , _rssi(RSSI_UNDEF)
    {}

    Oper(int state)
        : _selection_mode(-1)
        , _id(0)
        , _act(-1)
        , _state(state)
        , _ber(BER_UNDEF)
        , _rssi(RSSI_UNDEF)
    {}

    Oper( const Oper &oper )
        : _selection_mode(oper.get_selection_mode())
        , _id(oper.get_id())
        , _act(oper.get_act())
        , _state(oper.get_state())
        , _ber(oper.get_ber())
        , _rssi(oper.get_rssi())
    {}

    ~Oper() {}

    Oper& operator=( const Oper& oper )
    {
        _selection_mode = oper.get_selection_mode();
        _id    = oper.get_id();
        _act   = oper.get_act();
        _state = oper.get_state();
        _ber   = oper.get_ber();
        _rssi  = oper.get_rssi();
        return *this;
    }

    bool is_valid() const   { return ((_id > 0) && (_act >= 0)); }
    bool is_current() const { return (OPER_STATE_CURRENT == _state); }
    bool is_allowed() const { return ((OPER_STATE_CURRENT == _state) || (OPER_STATE_ALLOWED == _state)); }

    void clear()
    {
        _selection_mode=-1;
        _id=0;
        _act=-1;
        _state=OPER_STATE_UNKNOWN;
        _ber=BER_UNDEF;
        _rssi=RSSI_UNDEF;
    }

    int  get_selection_mode() const { return _selection_mode; }
    int  get_id() const             { return _id; }
    int  get_act() const            { return _act; }
    int  get_state() const          { return _state; }
    int  get_ber() const            { return _ber; }
    int  get_rssi() const           { return _rssi; }
    bool is_type_3g() const
    {
      bool result;
      switch(_act)
      {
        case 2: /*UMTS*/
        case 4: /*HSDPA*/
        case 5: /*HSUPA*/
        case 6: /*HSPA*/
          result = true;
          break;
      default:
          result = false;
          break;
      }
      return result;
    }


    int get_quality_percent() const
    {
      int result = 0;
      if (is_valid())
      {
        result = (((_rssi > RSSI_MAX) || (_rssi < 0)) ? 0 : ((_rssi * 100) / RSSI_MAX));
      }
      return result;
    }

    int get_quality_step() const
    {
      int result = 0;
      if (is_valid())
      {
        int asu = ((_rssi > RSSI_MAX) || (_rssi < 0)) ? 0 : _rssi;
        result = (is_type_3g() ? asu_mapping_3g[asu] : asu_mapping_2g[asu]);
      }
      return result;
    }

    void set_selection_mode( int selection_mode )
    {
      _selection_mode = selection_mode;
    }

    void set_id( int id )
    {
      _id = id;
    }

    void set_act( int act )
    {
      _act = act;
    }

    void set_csq( int ber, int rssi )
	  {
	    _ber = ber;
	    _rssi = rssi;
	  }

    void clear_csq()
    {
      _ber = BER_UNDEF;
      _rssi = RSSI_UNDEF;
    }

    void set_current()
    {
      _state = OPER_STATE_CURRENT;
    }

    void allow_permit()
    {
      _state = OPER_STATE_ALLOWED;
    }

    void deny_permit()
    {
      _state = OPER_STATE_DENIED;
    }

};

#endif
