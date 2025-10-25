// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <optional>

#include <functional>

#include <chrono>

module pragma.util;

import :clock;

static util::Clock::time_point g_startTime = util::Clock::now();
void util::clock::set_start_time(Clock::time_point t) { g_startTime = t; }
util::Clock::time_point util::clock::get_start_time() { return g_startTime; }
util::Clock::duration util::clock::get_duration_since_start() { return get_duration_since_start(Clock::now()); }
util::Clock::duration util::clock::get_duration_since_start(Clock::time_point t) { return t - g_startTime; }
uint64_t util::clock::to_int(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count(); }
util::Clock::duration util::clock::from_int(uint64_t dur) { return std::chrono::nanoseconds {dur}; }
long double util::clock::to_seconds(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() / static_cast<long double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds {1}).count()); }
long double util::clock::to_milliseconds(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() / static_cast<long double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds {1}).count()); }
uint64_t util::clock::to_nanoseconds(Clock::duration dur) { return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count(); }
