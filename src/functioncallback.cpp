// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "sharedutils/functioncallback.h"

bool TCallback::operator==(void *) const { return false; }
bool TCallback::operator!=(void *) const { return true; }
void TCallback::operator()() {}
void TCallback::Release() { delete this; }

/////////////////////////////

void CallbackHandle::operator()()
{
	if(!IsValid())
		return;
	auto cb = get();
	(*cb)();
}

void CallbackHandle::Remove()
{
	if(!IsValid())
		return;
	Invalidate();
}
