// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_LOG_HPP__
#define __UTIL_LOG_HPP__

#include "utildefinitions.h"
#include <cinttypes>
#include <functional>
#include <string>

namespace util {
	enum class LogSeverity : uint32_t {
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Critical,
		Disabled,

		Count
	};

	using LogHandler = std::function<void(const std::string &, util::LogSeverity)>;
	extern DLLSHUTIL std::string LOG_NL;
};

#endif
