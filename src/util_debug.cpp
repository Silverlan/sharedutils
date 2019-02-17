/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_debug.h"

#ifdef _WIN32
#include <sstream>
#include <vector>
#include <array>

#undef max

#pragma comment(lib,"DbgHelp.lib")

util::Symbol::LineInfo::LineInfo()
	: line(std::numeric_limits<decltype(line)>::max())
{}

util::Symbol::Symbol()
{
	info = std::shared_ptr<SYMBOL_INFO>(reinterpret_cast<SYMBOL_INFO*>(calloc(sizeof(SYMBOL_INFO) +256 *sizeof(uint8_t),1)),[](SYMBOL_INFO *p) {
		free(p);
	});
	info->MaxNameLen = 255;
	info->SizeOfStruct = sizeof(SYMBOL_INFO);
}

std::vector<util::Symbol> util::get_stack_backtrace(uint32_t maxIterations)
{
	std::vector<Symbol> symbols;
	std::vector<void*> stack(maxIterations,nullptr);
	auto process = GetCurrentProcess();
	SymSetOptions(SYMOPT_LOAD_LINES);
	SymInitialize(process,nullptr,true);
	auto frames = CaptureStackBackTrace(0,static_cast<DWORD>(stack.size()),stack.data(),nullptr);
	symbols.reserve(frames);

#if defined(_M_X64) || defined(__amd64__)
	_IMAGEHLP_LINE64 lineInfo;
#else
	_IMAGEHLP_LINE lineInfo;
#endif
	for(auto i=decltype(frames){0};i<frames;++i)
	{
		symbols.push_back(Symbol{});
		auto &symbol = symbols.back();
		SymFromAddr(process,reinterpret_cast<DWORD64>(stack[i]),0,symbol.info.get());
		DWORD dwDisplacement = 0;
		auto r = SymGetLineFromAddr(process,symbol.info->Address,&dwDisplacement,&lineInfo);
		if(r == TRUE)
		{
			symbol.lineInfo.line = lineInfo.LineNumber;
			symbol.lineInfo.path = lineInfo.FileName;
		}
	}
	return symbols;
}

std::string util::get_formatted_stack_backtrace_string(uint32_t maxIterations)
{
	std::stringstream msg;
	auto stack = get_stack_backtrace(maxIterations);
	uint32_t idx = 0;
	auto numSymbols = stack.size();
	for(auto &symbol : stack)
	{
		msg<<idx<<" - "<<symbol.info->Name;
		if(symbol.lineInfo.line < std::numeric_limits<decltype(symbol.lineInfo.line)>::max())
			msg<<" (Line "<<symbol.lineInfo.line<<")";
		msg<<"\n";
		if(!symbol.lineInfo.path.empty())
		{
			msg<<"   "<<symbol.lineInfo.path;
			if(idx < (numSymbols -1))
				msg<<"\n\n";
		}
		++idx;
	}
	return msg.str();
}

bool _impl::util_assert(const std::string &file,uint32_t line,const std::string &message,bool bAllowExit)
{
	std::stringstream msg;
	msg<<"Assertion failed in "<<file<<" on line "<<line<<":\n"<<message<<"\n\n";
	
	msg<<"Stack Trace:\n";
	msg<<util::get_formatted_stack_backtrace_string();
	UINT msgType = MB_OK;
	if(bAllowExit == true)
	{
		msgType = MB_YESNO;
		msg<<"\n\nExit?";
	}
    if(::MessageBoxA(nullptr,msg.str().c_str(),nullptr,msgType) == IDYES)
	{
        ::PostQuitMessage(EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	return true;
}

#endif