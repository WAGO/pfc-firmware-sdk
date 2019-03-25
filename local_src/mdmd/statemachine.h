// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#ifndef STATEMACHNIE_H
#define STATEMACHNIE_H

#include <stdio.h>
#include "regex.h"
#include "dbus_server.h"
#include <map>
#include <list>
#include <memory>
#include <queue>
#include "mdmd_log.h"

class Event
{
    private:

    public:
	virtual bool match_timeout(int id) {
	    (void)id; //unused parameter
	    return false;
	}
  virtual bool match_dbus( MethodInvocation invocation ) {
      (void)invocation; //unused parameter
      return false;
  }
	virtual bool match_string( const std::string &text ) {
      (void)text; //unused parameter
	    return false;
	}
  virtual bool match_read( const std::string &text ) {
      (void)text; //unused parameter
      return false;
  }
	virtual ~Event() {}
};

class State
{
    private:
    std::string _id;
	std::string _name;
	bool _dbus_transition_allowed;

    public:
    State(const std::string &state_id, const std::string &state_name)
          : _id (state_id), _name (state_name), _dbus_transition_allowed(false)
  {}

    State(const std::string &state_id, const std::string &state_name, bool state_dbus_transition_allowed)
          : _id (state_id), _name (state_name), _dbus_transition_allowed(state_dbus_transition_allowed)
  {}

    const std::string & id() { return _id; }
  const std::string & name() { return _name; }
	bool dbus_transition_allowed() { return _dbus_transition_allowed; }

};

template <class SM>
class TransitionFunctor
{
    public:
	virtual ~TransitionFunctor() {}
	virtual bool operator () (SM &sm, State &src, State &dst, Event &ev) = 0;
};

template <class SM>
class GuardFunctor
{
    public:
	virtual ~GuardFunctor() {}
	virtual bool operator () (const SM &sm) = 0;
};

template <class SM>
class ActionFunctor
{
    public:
	virtual ~ActionFunctor() {}
	virtual void operator () (SM &sm, Event &ev) = 0;
};

template <class SM>
class Transition
{
    public:
	typedef TransitionFunctor<SM> transistion_functor_t;
	typedef GuardFunctor<SM> guard_functor_t;
    private:
	State &_src;
	State &_dst;

	std::shared_ptr<Event> _ev;


	std::shared_ptr<transistion_functor_t> _function;
	std::shared_ptr<guard_functor_t> _guard;

    public:
	Transition(State &src, Event *ev, State &dst, transistion_functor_t *tf, guard_functor_t *g)
	    : _src (src)
	    , _dst (dst)
      , _ev (ev)
	    , _function (tf)
	    , _guard (g)
	{}

	~Transition() {}

	State * get_dst() {
	    return &_dst;
	}

	bool match_src( State & match ) {
	    return &match == &_src;
	}

	bool match_string( const std::string & str ) {
	    return _ev->match_string( str );
	}

  bool match_read( const std::string & str ) {
      return _ev->match_read( str );
  }

	bool match_dbus( MethodInvocation invocation ) {
	    return _ev->match_dbus(invocation);
	}

	bool match_timeout(int id) {
	    return _ev->match_timeout(id);
	}

	bool transition( SM &sm ) {
	    return (*_function)( sm, _src, _dst, *_ev );
	}

	bool check_guard( const SM &sm ) {
	    return (*_guard)( sm );
	}
};

template <class SM>
class Action
{
    public:
	typedef ActionFunctor<SM> action_functor_t;
    private:
	std::shared_ptr<Event> _ev;

	std::shared_ptr<action_functor_t> _function;

    public:
	Action(Event *ev, action_functor_t *af)
	    : _ev (ev)
	    , _function (af)
	{}

	~Action() {}

	bool match_string( const std::string & str ) {
	    return _ev->match_string( str );
	}

  bool match_read( const std::string & str ) {
      return _ev->match_read( str );
  }

	bool match_dbus( MethodInvocation invocation ) {
	    return _ev->match_dbus(invocation);
	}

	bool match_timeout(int id) {
	    return _ev->match_timeout(id);
	}

	void action( SM &sm ) {
	    (*_function)( sm, *_ev );
	}
};

template <class SM>
class StateMachine
{
    public:
	typedef Transition<SM> transition_t;
	typedef Action<SM> action_t;
	typedef std::list<transition_t> transition_list_t;
	typedef std::list<action_t> action_list_t;
	State * get_reset_state() { return _reset_state; }

    private:
	transition_list_t &_transition_list;
	action_list_t &_action_list;
	State *_current;
	State *_reset_state;
	std::queue<MethodInvocation> _dbus_queue;
  std::queue<int> _timeout_queue;
  std::string _last_read;

    protected:
	void reset_statemachine()
	{
      if (_current != _reset_state)
      {
        mdmd_Log(MD_LOG_DBG, "%s: reset statemachine: transition to %s \"%s\"\n",
                 _current->id().c_str(),
                 _reset_state->id().c_str(),
                 _reset_state->name().c_str());
      }
      else
      {
        mdmd_Log(MD_LOG_DBG, "%s: reset statemachine\n",
                 _current->id().c_str());
      }
      _current = _reset_state;

	    while (!_timeout_queue.empty())
      {
        _timeout_queue.pop();
      }
      //keep dbus events in queue (need to reply)
	}

    public:
	StateMachine( transition_list_t & tl, State & start, action_list_t & al )
	    : _transition_list (tl)
	    , _action_list (al)
	    , _current (& start)
	    , _reset_state(& start)
	{}

	~StateMachine() {}

	const std::string & get_last_read() const { return _last_read; }

	void do_dbus(MethodInvocation invocation, bool is_post_transition)
	{
	    for (auto i=_action_list.begin(); i!=_action_list.end(); i++)
	    {
	    	if (i->match_dbus(invocation))
	    	{
	    	  mdmd_Log(MD_LOG_DBG, "%s: event (dbus) \"%s\": do action\n",
	    	           _current->id().c_str(),
	    	           invocation.method_name().c_str());
	    		i->action( *static_cast<SM *>(this) );
          //Note: return here because invocation is not longer valid when finished by action
          return;
	    	}
	    }

      if ( !_current->dbus_transition_allowed() )
      {
        _dbus_queue.push(invocation);
        mdmd_Log(MD_LOG_DBG, "%s: event (dbus) \"%s\": enqueued\n",
                 _current->id().c_str(),
                 invocation.method_name().c_str());
        return;
      }

      for (auto i=_transition_list.begin(); i!=_transition_list.end(); i++)
	    {
	    	if (i->match_src( *_current ))
	    	{
	    		if (i->match_dbus(invocation))
	    		{
	    		  if (i->check_guard( *static_cast<SM *>(this) ))
	    		  {
	    			  mdmd_Log(MD_LOG_DBG, "%s: event (dbus) \"%s\": do transition\n",
	    			           _current->id().c_str(),
	    			           invocation.method_name().c_str());
              if (i->transition( *static_cast<SM *>(this) ))
              {
                if (_current != i->get_dst())
                {
                  mdmd_Log(MD_LOG_DBG, "%s: event (dbus) \"%s\": new state %s \"%s\"\n",
                           _current->id().c_str(),
                           invocation.method_name().c_str(),
                           i->get_dst()->id().c_str(),
                           i->get_dst()->name().c_str());
                }
                _current = i->get_dst();
              }
              //when transition done, then check for possibly enqueued events
              if ( !is_post_transition ) post_transition();
              return;
	    		  }
	    		}
	    	}
	    }

      mdmd_Log(MD_LOG_WRN, "%s: event (dbus) \"%s\": not handled\n",
               _current->id().c_str(),
               invocation.method_name().c_str());
      invocation.return_error("de.wago.mdmdError", "NOT ALLOWED");
	}

	void do_string( const std::string &str )
	{
	    bool action_done = false;

	    if (0==str.length()) return;

	    for (auto i=_action_list.begin(); i!=_action_list.end(); i++)
	    {
	    	if (i->match_string( str ))
	    	{
	    	  mdmd_Log(MD_LOG_DBG, "%s: event (strg) \"%s\": do action\n",
	    	           _current->id().c_str(),
	    	           str.c_str());
	    	  i->action( *static_cast<SM *>(this) );
	    	  action_done = true;
	    	  break; //continue, some events may trigger additionally a transition
	    	}
	    }

	    for (auto i=_transition_list.begin(); i!=_transition_list.end(); i++)
	    {
	    	if (i->match_src( *_current ))
	    	{
	    		if (i->match_string( str ))
	    		{
	    		  if (i->check_guard( *static_cast<SM *>(this) ))
	    		  {
	    			  mdmd_Log(MD_LOG_DBG, "%s: event (strg) \"%s\": do transition\n",
	    			           _current->id().c_str(),
	    			           str.c_str());
              if (i->transition( *static_cast<SM *>(this) ))
              {
                if (_current != i->get_dst())
                {
                  mdmd_Log(MD_LOG_DBG, "%s: event (strg) \"%s\": new state %s \"%s\"\n",
                           _current->id().c_str(),
                           str.c_str(),
                           i->get_dst()->id().c_str(),
                           i->get_dst()->name().c_str());
                }
                _current = i->get_dst();
              }
              //when transition done, then check for possibly enqueued events
              post_transition();
              return;
	    		  }
	    		}
	    	}
	    }
	    if (!action_done)
	    {
        mdmd_Log(MD_LOG_WRN, "%s: event (strg) \"%s\": not handled\n",
                 _current->id().c_str(),
                 str.c_str());
	    }
	}

  void do_read( const std::string &str )
  {
      bool action_done = false;

      if (0==str.length()) return;

      _last_read = str;

      for (auto i=_action_list.begin(); i!=_action_list.end(); i++)
      {
        if (i->match_read( str ))
        {
          mdmd_Log(MD_LOG_DBG, "%s: event (read) \"%s\": do action\n",
                   _current->id().c_str(),
                   str.c_str());
          i->action( *static_cast<SM *>(this) );
          action_done = true;
          break; //continue, some events may trigger additionally a transition
        }
      }

      for (auto i=_transition_list.begin(); i!=_transition_list.end(); i++)
      {
        if (i->match_src( *_current ))
        {
          if (i->match_read( str ))
          {
            if (i->check_guard( *static_cast<SM *>(this) ))
            {
              mdmd_Log(MD_LOG_DBG, "%s: event (read) \"%s\": do transition\n",
                       _current->id().c_str(),
                       str.c_str());
              if (i->transition( *static_cast<SM *>(this) ))
              {
                if (_current != i->get_dst())
                {
                  mdmd_Log(MD_LOG_DBG, "%s: event (read) \"%s\": new state %s \"%s\"\n",
                           _current->id().c_str(),
                           str.c_str(),
                           i->get_dst()->id().c_str(),
                           i->get_dst()->name().c_str());
                }
                _current = i->get_dst();
              }
              //when transition done, then check for possibly enqueued events
              post_transition();
              return;
            }
          }
        }
      }
      if (!action_done)
      {
        mdmd_Log(MD_LOG_WRN, "%s: event (read) \"%s\": not handled\n",
                 _current->id().c_str(),
                 str.c_str());
      }
  }

	void do_timeout( int id, bool is_post_transition )
	{
      if (! _current->dbus_transition_allowed() && (id != 0))
      {
        _timeout_queue.push(id);
        mdmd_Log(MD_LOG_DBG, "%s: event (timeout) %d: enqueued\n",
                 _current->id().c_str(),
                 id);
        return;
      }

	    for (auto i=_action_list.begin(); i!=_action_list.end(); i++)
	    {
	    	if (i->match_timeout(id))
	    	{
	        mdmd_Log(MD_LOG_DBG, "%s: event (timeout) %d: do action\n",
	                 _current->id().c_str(),
	                 id);
	    		i->action( *static_cast<SM *>(this) );
          return;
	    	}
	    }

	    for (auto i=_transition_list.begin(); i!=_transition_list.end(); i++)
	    {
	    	if (i->match_src( *_current ))
	    	{
	    		if (i->match_timeout( id ))
	    		{
	    		  if (i->check_guard( *static_cast<SM *>(this) ))
	    		  {
              mdmd_Log(MD_LOG_DBG, "%s: event (timeout) %d: do transition\n",
                       _current->id().c_str(),
                       id);
              if (i->transition( *static_cast<SM *>(this) ))
              {
                if (_current != i->get_dst())
                {
                  mdmd_Log(MD_LOG_DBG, "%s: event (timeout) %d: new state %s \"%s\"\n",
                           _current->id().c_str(),
                           id,
                           i->get_dst()->id().c_str(),
                           i->get_dst()->name().c_str());
                }
                _current = i->get_dst();
              }
              //when transition done, then check for possibly enqueued events
              if ( !is_post_transition ) post_transition();
              return;
	    		  }
	    		}
	    	}
	    }

      mdmd_Log(MD_LOG_INF, "%s: event (timeout) %d: not handled\n",
               _current->id().c_str(),
               id);
	}

	void post_transition() {
      while(_current->dbus_transition_allowed())
      {
        if (!_timeout_queue.empty())
        {
          int id = _timeout_queue.front();
          _timeout_queue.pop();
          mdmd_Log(MD_LOG_DBG, "%s: event (timeout) %d: dequeued\n",
                   _current->id().c_str(),
                   id);
          this->do_timeout( id, true );
          continue;
        }
        if (!_dbus_queue.empty())
        {
          MethodInvocation mi = _dbus_queue.front();
          _dbus_queue.pop();
          mdmd_Log(MD_LOG_DBG, "%s: event (dbus) \"%s\": dequeued\n",
                   _current->id().c_str(),
                   mi.method_name().c_str());
          this->do_dbus( mi, true );
          continue;
        }
        break;
      }
	}

	const std::string & get_state() const { return _current->id(); }
	const std::string & get_state_name() const { return _current->name(); }

};


#endif // STATEMACHNIE_H
