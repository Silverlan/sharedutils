/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/functioncallback.h"

bool TCallback::operator==(void*) {return false;}
bool TCallback::operator!=(void*) {return true;}
void TCallback::operator()() {}
void TCallback::Release() {delete this;}

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
