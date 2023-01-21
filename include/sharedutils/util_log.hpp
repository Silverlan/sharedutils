/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_LOG_HPP__
#define __UTIL_LOG_HPP__

#include <cinttypes>
#include <functional>
#include <string>

namespace util
{
	enum class LogSeverity : uint32_t
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Critical,
		Disabled,

		Count
	};

	using LogHandler = std::function<void(const std::string&,util::LogSeverity)>;
};

#endif
