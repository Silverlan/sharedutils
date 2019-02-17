/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_COMMAND_MANAGER_HPP__
#define __UTIL_COMMAND_MANAGER_HPP__

#include "utildefinitions.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

namespace util
{
	class DLLSHUTIL CommandManager
	{
	private:
		static std::vector<std::string> m_cmds;
		static std::vector<std::function<void(std::vector<std::string>&)>> m_functions;
		static bool Input(std::string *cmdOut,const std::function<void(const std::function<void(std::vector<std::string>&)>&,std::vector<std::string>&)> &callbac);
	public:
		static void RegisterCommand(const char *cmd,const std::function<void(std::vector<std::string>&)> &f);
		static bool Input(std::string *cmdOut=nullptr);
		static const std::vector<std::string> &GetCommands();

		// Optional: Run command manager on separate thread
		static void StartAsync();
		static void PollEvents();
		static bool ShouldExit();
	};
};

#endif