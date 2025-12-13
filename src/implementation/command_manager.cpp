// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef _WIN32
#include <Windows.h>
#endif

module pragma.util;

import :command_manager;

using namespace pragma::util;

std::vector<std::string> CommandManager::m_cmds;
std::vector<std::function<void(std::vector<std::string> &)>> CommandManager::m_functions;

void CommandManager::RegisterCommand(const char *cmd, const std::function<void(std::vector<std::string> &)> &f)
{
	std::string scmd = cmd;
	std::transform(scmd.begin(), scmd.end(), scmd.begin(), [](unsigned char c) -> char { return static_cast<char>(std::tolower(c)); });
	m_cmds.push_back(scmd);
	m_functions.push_back(f);
}

bool CommandManager::Input(std::string *cmdOut, const std::function<void(const std::function<void(std::vector<std::string> &)> &, std::vector<std::string> &)> &callback)
{
	std::string s;
	std::getline(std::cin, s);
	std::vector<std::string> args;
	string::explode(s, string::WHITESPACE.c_str(), args);
	if(args.empty())
		return true;
	std::string cmd = args.front();
	if(cmd.empty())
		return true;
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) -> char { return static_cast<char>(std::tolower(c)); });
	if(cmdOut != nullptr)
		*cmdOut = cmd;
	args.erase(args.begin());
	auto it = std::find_if(m_cmds.begin(), m_cmds.end(), [&cmd](const std::string &cmdOther) { return cmdOther == cmd; });
	if(it == m_cmds.end())
		return false;
	callback(m_functions.at(it - m_cmds.begin()), args);
	return true;
}

bool CommandManager::Input(std::string *cmdOut)
{
	return Input(cmdOut, [](const std::function<void(std::vector<std::string> &)> &fInput, std::vector<std::string> &args) { fInput(args); });
}

const std::vector<std::string> &CommandManager::GetCommands() { return m_cmds; }

static std::thread s_thread;
static std::queue<std::function<void(void)>> s_eventQueue;
static std::mutex s_eventMutex;
static std::atomic<bool> s_shouldExit = {false};
void CommandManager::StartAsync()
{
	s_thread = std::thread([]() {
		std::string cmd;
		for(;;) {
			auto r = Input(&cmd, [](const std::function<void(std::vector<std::string> &)> &fInput, std::vector<std::string> &args) {
				s_eventMutex.lock();
				s_eventQueue.push([fInput, args]() mutable { fInput(args); });
				s_eventMutex.unlock();
			});
			if(s_shouldExit)
				break;
			if(r == false) {
				if(cmd != "exit")
					std::cout << "Unknown command '" << cmd << "'!" << std::endl;
				else
					break;
			}
		}
		s_shouldExit = true;
		// End of thread
	});
}
void CommandManager::Stop(bool wait)
{
	s_shouldExit = true;
	if(wait && s_thread.joinable())
		s_thread.join();
}
void CommandManager::PollEvents()
{
	s_eventMutex.lock();
	while(s_eventQueue.empty() == false) {
		s_eventQueue.front()();
		s_eventQueue.pop();
	}
	s_eventMutex.unlock();
}
bool CommandManager::ShouldExit() { return s_shouldExit; }
void CommandManager::Join()
{
	if(!s_thread.joinable())
		return;
	s_thread.detach();
#ifdef _WIN32
	INPUT_RECORD input;
	unsigned long numEvents;
	WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input, 0, &numEvents); // Workaround: Writes to the console to make sure the thread can end properly
#endif
}
