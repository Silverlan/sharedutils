/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_CLOCK_HPP__
#define __UTIL_CLOCK_HPP__

#include "sharedutils/utildefinitions.h"
#include <chrono>
#include <cinttypes>

namespace util
{
	using Clock = std::chrono::steady_clock;
	namespace clock
	{
		DLLSHUTIL void set_start_time(Clock::time_point t);
		DLLSHUTIL Clock::time_point get_start_time();
		DLLSHUTIL Clock::duration get_duration_since_start();
		DLLSHUTIL Clock::duration get_duration_since_start(Clock::time_point t);
		DLLSHUTIL uint64_t to_int(Clock::duration dur);
		DLLSHUTIL Clock::duration from_int(uint64_t dur);
		DLLSHUTIL long double to_seconds(Clock::duration dur);
		DLLSHUTIL long double to_milliseconds(Clock::duration dur);
		DLLSHUTIL uint64_t to_nanoseconds(Clock::duration dur);
	}
};

#endif
