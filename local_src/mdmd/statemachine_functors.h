// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef STATEMACHINE_FUNCTORS_H
#define STATEMACHINE_FUNCTORS_H

template <class SM>
class TransitionFunctionFunctor : public SM::transition_t::transistion_functor_t
{
    private:
	typedef bool (*transition_function_t)(SM &sm, State &src, State &dst, Event &ev);
	transition_function_t _tf;
    public:
	TransitionFunctionFunctor( transition_function_t tf )
	    : _tf(tf)
	{}

	~TransitionFunctionFunctor() {}

	bool operator() (SM &sm, State &src, State &dst, Event &ev) {
	    return _tf(sm,src,dst,ev);
	}
};

template <class SM>
class ActionFunctionFunctor : public SM::action_t::action_functor_t
{
    private:
	typedef void (*action_function_t)(SM &sm, Event &ev);
	action_function_t _af;
    public:
	ActionFunctionFunctor( action_function_t af )
	    : _af(af)
	{}

	~ActionFunctionFunctor() {}

	void operator() (SM &sm, Event &ev) {
	    _af(sm,ev);
	}
};

template <class SM>
class GuardFunctionFunctor : public SM::transition_t::guard_functor_t
{
    private:
	typedef bool (*guard_function_t)(const SM &sm);
	guard_function_t _gf;
    public:
	GuardFunctionFunctor( guard_function_t gf )
	    : _gf(gf)
	{}

	~GuardFunctionFunctor() {}

	bool operator() (const SM &sm) {
	    return _gf(sm);
	}
};

template <class SM>
class positive_guard : public SM::transition_t::guard_functor_t
{
    public:
	bool operator() (const SM &sm) { (void)sm; /*unused parameter*/ return true; }
	~positive_guard() {}
};

template <class SM>
class sm_write : public SM::transition_t::transistion_functor_t
{
    private:
	std::string _text;
    public:
	sm_write( const std::string &text )
	    : _text(text)
	{}
	bool operator () (SM &sm, State &src, State &dst, Event &ev) {
	    sm.write(_text);
	    return true;
	}
};

#endif
