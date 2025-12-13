// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.util;

import :clock;

static pragma::util::Clock::time_point g_startTime = pragma::util::Clock::now();
void pragma::util::clock::set_start_time(Clock::time_point t) { g_startTime = t; }
pragma::util::Clock::time_point pragma::util::clock::get_start_time() { return g_startTime; }
pragma::util::Clock::duration pragma::util::clock::get_duration_since_start() { return get_duration_since_start(Clock::now()); }
pragma::util::Clock::duration pragma::util::clock::get_duration_since_start(Clock::time_point t) { return t - g_startTime; }
uint64_t pragma::util::clock::to_int(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count(); }
pragma::util::Clock::duration pragma::util::clock::from_int(uint64_t dur) { return std::chrono::nanoseconds {dur}; }
long double pragma::util::clock::to_seconds(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() / static_cast<long double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds {1}).count()); }
long double pragma::util::clock::to_milliseconds(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() / static_cast<long double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds {1}).count()); }
uint64_t pragma::util::clock::to_nanoseconds(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count(); }
