// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:log;

export import std.compat;

export namespace util {
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
#ifdef _WIN32
	DLLSHUTIL const std::string LOG_NL = "\r\n";
#else
	DLLSHUTIL const std::string LOG_NL = "\n";
#endif
};
