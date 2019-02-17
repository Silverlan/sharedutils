/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_DEBUG_H__
#define __UTIL_DEBUG_H__

#include <string>
#include "utildefinitions.h"

#ifdef _WIN32
namespace _impl
{
	DLLSHUTIL bool util_assert(const std::string &file,uint32_t line,const std::string &message="",bool bAllowExit=true);
};
#define UTIL_ASSERT(f, ...) (void) ((f) || !_impl::util_assert(__FILE__,__LINE__, ##__VA_ARGS__))

#include <memory>
#include <Windows.h>
#include <DbgHelp.h>
#include <vector>

namespace util
{
	struct DLLSHUTIL Symbol
	{
		struct DLLSHUTIL LineInfo
		{
			LineInfo();
			std::string path;
			uint32_t line;
		};
		Symbol();
		std::shared_ptr<SYMBOL_INFO> info;
		LineInfo lineInfo;
	};
	DLLSHUTIL std::vector<Symbol> get_stack_backtrace(uint32_t maxIterations=100);
	DLLSHUTIL std::string get_formatted_stack_backtrace_string(uint32_t maxIterations=100);
};

#else
#define UTIL_ASSERT(f)
#endif

#endif
