// SPDX-FileCopyrightText: (c) 2023 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"

export module pragma.util:log;

export import std.compat;

export namespace pragma::util {
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

	using LogHandler = std::function<void(const std::string &, pragma::util::LogSeverity)>;
#ifdef _WIN32
	constexpr std::string_view LOG_NL = "\r\n";
#else
	constexpr std::string_view LOG_NL = "\n";
#endif
};
