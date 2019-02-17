/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_command_manager.hpp"
#include "sharedutils/util_string.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

using namespace util;

std::vector<std::string> CommandManager::m_cmds;
std::vector<std::function<void(std::vector<std::string>&)>> CommandManager::m_functions;

void CommandManager::RegisterCommand(const char *cmd,const std::function<void(std::vector<std::string>&)> &f)
{
	std::string scmd = cmd;
	std::transform(scmd.begin(),scmd.end(),scmd.begin(),::tolower);
	m_cmds.push_back(scmd);
	m_functions.push_back(f);
}

bool CommandManager::Input(std::string *cmdOut,const std::function<void(const std::function<void(std::vector<std::string>&)>&,std::vector<std::string>&)> &callback)
{
	std::string s;
	std::getline(std::cin,s);
	std::vector<std::string> args;
	ustring::explode(s,ustring::WHITESPACE.c_str(),args);
	if(args.empty())
		return true;
	std::string cmd = args.front();
	if(cmd.empty())
		return true;
	std::transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);
	if(cmdOut != nullptr)
		*cmdOut = cmd;
	args.erase(args.begin());
	auto it = std::find_if(m_cmds.begin(),m_cmds.end(),[&cmd](const std::string &cmdOther) {
		return cmdOther == cmd;
	});
	if(it == m_cmds.end())
		return false;
	callback(m_functions.at(it -m_cmds.begin()),args);
	return true;
}

bool CommandManager::Input(std::string *cmdOut)
{
	return Input(cmdOut,[](const std::function<void(std::vector<std::string>&)> &fInput,std::vector<std::string> &args) {
		fInput(args);
	});
}

const std::vector<std::string> &CommandManager::GetCommands() {return m_cmds;}

static std::thread s_thread;
static std::queue<std::function<void(void)>> s_eventQueue;
static std::mutex s_eventMutex;
static std::atomic<bool> s_shouldExit = {false};
void CommandManager::StartAsync()
{
	s_thread = std::thread([]() {
		std::string cmd;
		for(;;)
		{
			auto r = Input(&cmd,[](const std::function<void(std::vector<std::string>&)> &fInput,std::vector<std::string> &args) {
				s_eventMutex.lock();
					s_eventQueue.push([fInput,args]() mutable {
						fInput(args);
					});
				s_eventMutex.unlock();
			});
			if(r == false)
			{
				if(cmd != "exit")
					std::cout<<"Unknown command '"<<cmd<<"'!"<<std::endl;
				else
					break;
			}
		}
		s_shouldExit = true;
	});
}
void CommandManager::PollEvents()
{
	s_eventMutex.lock();
		while(s_eventQueue.empty() == false)
		{
			s_eventQueue.front()();
			s_eventQueue.pop();
		}
	s_eventMutex.unlock();
}
bool CommandManager::ShouldExit()
{
	return s_shouldExit;
}
