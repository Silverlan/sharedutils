// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/utildefinitions.h"
#include <chrono>
#include <cinttypes>

export module pragma.util:clock;

export namespace util {
	using Clock = std::chrono::steady_clock;
	namespace clock {
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
