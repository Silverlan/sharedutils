// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <sstream>
#include <vector>
#include <array>
#include <mutex>
#include <optional>
#include <cpptrace/cpptrace.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#endif

module pragma.util;

import :debug;

static std::array<std::string, static_cast<uint32_t>(util::debug::MessageBoxButton::Count)> g_buttonLabels = {
  "Ok",
  "Cancel",
  "Abort",
  "Retry",
  "Ignore",
  "Yes",
  "No",
  "Try Again",
  "Continue",
};
void util::debug::set_button_labels(const std::array<std::string, static_cast<uint32_t>(MessageBoxButton::Count)> &labels) { g_buttonLabels = labels; }

std::optional<util::debug::MessageBoxButton> util::debug::show_message_prompt(const std::string &msg, MessageBoxButtons bts, std::optional<std::string> title)
{
	if(!title)
		title = util::get_program_name();
#ifdef _WIN32
	uint32_t winBt = 0;
	switch(bts) {
	case MessageBoxButtons::Ok:
		winBt = MB_OK;
		break;
	case MessageBoxButtons::OkCancel:
		winBt = MB_OKCANCEL;
		break;
	case MessageBoxButtons::AbortRetryIgnore:
		winBt = MB_ABORTRETRYIGNORE;
		break;
	case MessageBoxButtons::YesNoCancel:
		winBt = MB_YESNOCANCEL;
		break;
	case MessageBoxButtons::YesNo:
		winBt = MB_YESNO;
		break;
	case MessageBoxButtons::RetryCancel:
		winBt = MB_RETRYCANCEL;
		break;
	case MessageBoxButtons::CancelTryAgainContinue:
		winBt = MB_CANCELTRYCONTINUE;
		break;
	}

	auto wmsg = ustring::string_to_wstring(msg);
	auto wtitle = ustring::string_to_wstring(*title);
	auto res = ::MessageBoxW(NULL, wmsg.c_str(), wtitle.c_str(), winBt);
	switch(res) {
	case IDOK:
		return MessageBoxButton::Ok;
	case IDCANCEL:
		return MessageBoxButton::Cancel;
	case IDABORT:
		return MessageBoxButton::Abort;
	case IDRETRY:
		return MessageBoxButton::Retry;
	case IDIGNORE:
		return MessageBoxButton::Ignore;
	case IDYES:
		return MessageBoxButton::Yes;
	case IDNO:
		return MessageBoxButton::No;
	case IDTRYAGAIN:
		return MessageBoxButton::TryAgain;
	case IDCONTINUE:
		return MessageBoxButton::Continue;
	default:
		return {};
	}
	return {};
#else
	std::vector<MessageBoxButton> buttons;
	buttons.reserve(3);
	switch(bts) {
	case MessageBoxButtons::Ok:
		buttons.push_back(MessageBoxButton::Ok);
		break;
	case MessageBoxButtons::OkCancel:
		buttons.push_back(MessageBoxButton::Ok);
		buttons.push_back(MessageBoxButton::Cancel);
		break;
	case MessageBoxButtons::AbortRetryIgnore:
		buttons.push_back(MessageBoxButton::Abort);
		buttons.push_back(MessageBoxButton::Retry);
		buttons.push_back(MessageBoxButton::Ignore);
		break;
	case MessageBoxButtons::YesNoCancel:
		buttons.push_back(MessageBoxButton::Yes);
		buttons.push_back(MessageBoxButton::No);
		buttons.push_back(MessageBoxButton::Cancel);
		break;
	case MessageBoxButtons::YesNo:
		buttons.push_back(MessageBoxButton::Yes);
		buttons.push_back(MessageBoxButton::No);
		break;
	case MessageBoxButtons::RetryCancel:
		buttons.push_back(MessageBoxButton::Retry);
		buttons.push_back(MessageBoxButton::Cancel);
		break;
	case MessageBoxButtons::CancelTryAgainContinue:
		buttons.push_back(MessageBoxButton::Cancel);
		buttons.push_back(MessageBoxButton::TryAgain);
		buttons.push_back(MessageBoxButton::Continue);
		break;
	}

	if(buttons.empty())
		return {};
	std::stringstream cmd;
	cmd << "zenity ";
	if(buttons.size() == 1)
		cmd << "--info ";
	else
		cmd << "--question ";
	cmd << "--title='" + *title + "' ";
	cmd << "--text='" + msg + "' ";

	auto getButtonText = [](MessageBoxButton button) -> std::string {
		auto identifier = ustring::to_snake_case(std::string {magic_enum::enum_name(button)});
		auto text = g_buttonLabels[static_cast<uint32_t>(button)];
		return text;
	};
	cmd << "--ok-label='" << getButtonText(buttons[0]) << "' ";
	if(buttons.size() > 1) {
		cmd << "--cancel-label='" << getButtonText(buttons[1]) << "' ";
		if(buttons.size() > 2) {
			cmd << "--extra-button='" << getButtonText(buttons[2]) << "' ";
		}
	}

	int result = system(cmd.str().c_str());
	if(result < 0)
		return {};
	int code = WEXITSTATUS(result);
	if(code >= buttons.size())
		return {};
	return buttons[code];
#endif
}

//////////

static std::function<std::string()> g_luaBacktraceFunction;
void util::debug::set_lua_backtrace_function(const std::function<std::string()> &func) { g_luaBacktraceFunction = func; }

std::string util::debug::get_formatted_stack_backtrace_string(uint32_t maxIterations)
{
	auto trace = cpptrace::generate_trace(0, maxIterations).to_string(true);
	if(!g_luaBacktraceFunction || trace.find("in lua_pcall") == std::string::npos)
		return trace;
	std::stringstream msg;
	msg << trace << "\n\nLua Call Trace:\n";
	// If we're in lua_pcall, we can get a backtrace from Lua
	auto luaBacktrace = g_luaBacktraceFunction();
	if(!luaBacktrace.empty()) {
		msg << luaBacktrace;
		msg << "\n\n";
	}
	return msg.str();
}

bool _impl::util_assert(const std::string &file, uint32_t line, const std::string &message, bool bAllowExit)
{
	std::stringstream msg;
	msg << "Assertion failed in " << file << " on line " << line << ":\n" << message << "\n\n";

	msg << "Stack Trace:\n";
	msg << util::debug::get_formatted_stack_backtrace_string();
	auto msgType = util::debug::MessageBoxButtons::Ok;
	if(bAllowExit == true) {
		msgType = util::debug::MessageBoxButtons::YesNo;
		msg << "\n\nExit?";
	}
	auto bt = util::debug::show_message_prompt(msg.str(), msgType, "Assertion Failed");
	if(bt == util::debug::MessageBoxButton::Yes) {
#ifdef _WIN32
		::PostQuitMessage(EXIT_FAILURE);
#endif
		exit(EXIT_FAILURE);
		// Unreachable
		return false;
	}
	return true;
}

#ifdef _WIN32

#undef max

#pragma comment(lib, "DbgHelp.lib")

util::Symbol::LineInfo::LineInfo() : line(std::numeric_limits<decltype(line)>::max()) {}

util::Symbol::Symbol()
{
	info = std::shared_ptr<SYMBOL_INFO>(reinterpret_cast<SYMBOL_INFO *>(calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(uint8_t), 1)), [](SYMBOL_INFO *p) { free(p); });
	info->MaxNameLen = 255;
	info->SizeOfStruct = sizeof(SYMBOL_INFO);
}

// dbghelp functions are not thread safe
static std::mutex g_dbgHelpMutex;
std::vector<util::Symbol> util::get_stack_backtrace(uint32_t maxIterations)
{
	std::lock_guard<std::mutex> lock {g_dbgHelpMutex};
	std::vector<Symbol> symbols;
	std::vector<void *> stack(maxIterations, nullptr);
	auto process = GetCurrentProcess();
	SymSetOptions(SYMOPT_LOAD_LINES);
	SymInitialize(process, nullptr, true);
	auto frames = CaptureStackBackTrace(0, static_cast<DWORD>(stack.size()), stack.data(), nullptr);
	symbols.reserve(frames);

#if defined(_M_X64) || defined(__amd64__)
	_IMAGEHLP_LINE64 lineInfo;
#else
	_IMAGEHLP_LINE lineInfo;
#endif
	for(auto i = decltype(frames) {0}; i < frames; ++i) {
		symbols.push_back(Symbol {});
		auto &symbol = symbols.back();
		if(SymFromAddr(process, reinterpret_cast<DWORD64>(stack[i]), 0, symbol.info.get()) == FALSE) {
			symbols.pop_back();
			continue;
		}
		DWORD dwDisplacement = 0;
		auto r = SymGetLineFromAddr(process, symbol.info->Address, &dwDisplacement, &lineInfo);
		if(r == TRUE) {
			symbol.lineInfo.line = lineInfo.LineNumber;
			symbol.lineInfo.path = lineInfo.FileName;
		}
	}
	return symbols;
}

#endif
