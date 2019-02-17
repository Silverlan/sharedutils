/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/scope_guard.h"

ScopeGuard::ScopeGuard()
	: f(nullptr)
{}

ScopeGuard::ScopeGuard(ScopeGuard &&other)
	: f(std::move(other.f))
{
	other.f = nullptr;
}

ScopeGuard::~ScopeGuard()
{
	if(f)
		f(); // must not throw
}

void ScopeGuard::dismiss() throw() {
	f = nullptr;
}
