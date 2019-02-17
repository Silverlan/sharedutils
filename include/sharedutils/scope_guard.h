/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SCOPE_GUARD_H__
#define __SCOPE_GUARD_H__

#include "utildefinitions.h"
#include <functional>

class DLLSHUTIL ScopeGuard
{
private:
	std::function<void()> f;
public: 
	template<class Callable>
		ScopeGuard(Callable &&undo_func);
	ScopeGuard(ScopeGuard &&other);
	ScopeGuard();
	~ScopeGuard();
	void dismiss() throw();
	ScopeGuard(const ScopeGuard&)=delete;
	template<class Callable>
		ScopeGuard &operator=(Callable &&undo_func);
};

template<class Callable>
	ScopeGuard &ScopeGuard::operator=(Callable &&undo_func)
{
	if(f != nullptr)
		f();
	f = std::forward<Callable>(undo_func);
	return *this;
}


template<class Callable> 
	ScopeGuard::ScopeGuard(Callable && undo_func)
		: f(std::forward<Callable>(undo_func))
{}

#endif