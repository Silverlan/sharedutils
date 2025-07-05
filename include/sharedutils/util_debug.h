// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_DEBUG_H__
#define __UTIL_DEBUG_H__

#include "utildefinitions.h"
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <functional>

namespace util::debug {
	enum class MessageBoxButton : uint32_t {
		Ok = 0,
		Cancel,
		Abort,
		Retry,
		Ignore,
		Yes,
		No,
		TryAgain,
		Continue,
		Count,
	};
	enum class MessageBoxButtons : uint8_t {
		Ok = 0,
		OkCancel,
		AbortRetryIgnore,
		YesNoCancel,
		YesNo,
		RetryCancel,
		CancelTryAgainContinue,
	};
	DLLSHUTIL std::optional<MessageBoxButton> show_message_prompt(const std::string &msg, MessageBoxButtons bts, std::optional<std::string> title = {});
	DLLSHUTIL void set_button_labels(const std::array<std::string, static_cast<uint32_t>(MessageBoxButton::Count)> &labels);
};

namespace util::debug {
	DLLSHUTIL void set_lua_backtrace_function(const std::function<std::string()> &func);
	DLLSHUTIL std::string get_formatted_stack_backtrace_string(uint32_t maxIterations = 100);
};

namespace _impl {
	DLLSHUTIL bool util_assert(const std::string &file, uint32_t line, const std::string &message = "", bool bAllowExit = true);
};
#define UTIL_ASSERT(f, ...) (void)((f) || !_impl::util_assert(__FILE__, __LINE__, ##__VA_ARGS__))

#ifdef _WIN32

#include <Windows.h>
#include <DbgHelp.h>

namespace util {
	struct DLLSHUTIL Symbol {
		struct DLLSHUTIL LineInfo {
			LineInfo();
			std::string path;
			uint32_t line;
		};
		Symbol();
		std::shared_ptr<SYMBOL_INFO> info;
		LineInfo lineInfo;
	};
	DLLSHUTIL std::vector<Symbol> get_stack_backtrace(uint32_t maxIterations = 100);
};

#endif

#endif
