/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_EVENT_REPLY_HPP__
#define __UTIL_EVENT_REPLY_HPP__

#include <cinttypes>

namespace util
{
	enum class EventReply : uint8_t
	{
		Unhandled = 0,
		Handled
	};
};

#endif
