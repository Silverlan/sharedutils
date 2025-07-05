// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UTIL_COMMAND_MANAGER_HPP__
#define __UTIL_COMMAND_MANAGER_HPP__

#include "utildefinitions.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

namespace util {
	class DLLSHUTIL CommandManager {
	  private:
		static std::vector<std::string> m_cmds;
		static std::vector<std::function<void(std::vector<std::string> &)>> m_functions;
		static bool Input(std::string *cmdOut, const std::function<void(const std::function<void(std::vector<std::string> &)> &, std::vector<std::string> &)> &callbac);
	  public:
		static void RegisterCommand(const char *cmd, const std::function<void(std::vector<std::string> &)> &f);
		static bool Input(std::string *cmdOut = nullptr);
		static const std::vector<std::string> &GetCommands();

		// Optional: Run command manager on separate thread
		static void StartAsync();
		static void PollEvents();
		static bool ShouldExit();
		static void Stop(bool wait = true);
		static void Join();
	};
};

#endif
