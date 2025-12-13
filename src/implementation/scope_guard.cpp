// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :scope_guard;

using namespace pragma::util;

ScopeGuard::ScopeGuard() : f(nullptr) {}

ScopeGuard::ScopeGuard(ScopeGuard &&other) : f(std::move(other.f)) { other.f = nullptr; }

ScopeGuard &ScopeGuard::operator=(ScopeGuard &&other)
{
	if(f)
		f();
	f = std::move(other.f);
	other.f = nullptr;
	return *this;
}

ScopeGuard::~ScopeGuard()
{
	if(f)
		f(); // must not throw
}

void ScopeGuard::dismiss() throw() { f = nullptr; }
void ScopeGuard::release() throw()
{
	if(f)
		f(); // must not throw
	dismiss();
}
