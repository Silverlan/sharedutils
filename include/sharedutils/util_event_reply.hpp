// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_EVENT_REPLY_HPP__
#define __UTIL_EVENT_REPLY_HPP__

#include <cinttypes>

namespace util {
	enum class EventReply : uint8_t { Unhandled = 0, Handled };
};

#endif
